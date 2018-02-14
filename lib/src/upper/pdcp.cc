/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of the srsUE library.
 *
 * srsUE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsUE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */


#include "srslte/upper/pdcp.h"
#include <unistd.h>
#include <stdio.h>
#include <inttypes.h>
#include <sstream>

namespace srslte {

pdcp::pdcp()
{}

void pdcp::init(srsue::rlc_interface_pdcp *rlc_, srsue::rrc_interface_pdcp *_rrc[], srsue::gw_interface_pdcp *_gw[], log *pdcp_log_, uint8_t direction_)
{
  rlc       = rlc_;
  pdcp_log  = pdcp_log_;
  direction = direction_;

  //gines
  for(int slice = 0; slice < NUM_SLICES; ++slice){
    rrc[slice] = _rrc[slice];
    gw[slice] = _gw[slice];
    //slices[0][slice] = new  pdcp_entity();
    slices[0][slice].init(rlc, rrc[slice], gw[slice], pdcp_log, RB_ID_SRB0, direction);
  }
  //pdcp_log  = pdcp_log_;
  //direction = direction_;

  //pdcp_array[0].init(rlc, rrc[0], gw[0], pdcp_log, RB_ID_SRB0, direction); // SRB0
	
  connection_setup_pdu = new srslte::byte_buffer_t(); 

	current_slice = 0;

  data_ready = false;

  SLICE2 = 0;
}

void pdcp::stop()
{}

void pdcp::reset()
{
  for (int slice = 0; slice < NUM_SLICES; ++slice){
    for (int bearer = 0; bearer < SRSLTE_N_RADIO_BEARERS; ++bearer){
      slices[bearer][slice].reset();
    } 
  }

  for (int slice = 0; slice < NUM_SLICES; ++slice){
    slices[0][slice].init(rlc, rrc[slice], gw[slice], pdcp_log, RB_ID_SRB0, direction);
  }

}

/*******************************************************************************
  RRC/GW interface
*******************************************************************************/
void pdcp::write_sdu(int slice_id, uint32_t lcid, byte_buffer_t *sdu)
{
  pdcp_log->info("Write sdu slice_id=%i sdu_size=%i\n",slice_id, sdu->N_bytes); 
  if(valid_lcid(lcid))
    slices[lcid][slice_id].write_sdu(sdu);
}

// Starts next slice at the end of NAS process
void pdcp::write_sdu_and_next_slice(int slice_id, uint32_t lcid, byte_buffer_t *sdu)
{
  pdcp_log->info("Write sdu slice_id=%i sdu_size=%i\n",slice_id, sdu->N_bytes);
  if(valid_lcid(lcid))
    slices[lcid][slice_id].write_sdu(sdu);
}

// After SIB2 configure RRC state of the different slices.
void pdcp::write_sdu_uecri(int slice_id, uint64_t uecri, uint32_t lcid, byte_buffer_t *sdu)
{
  
  if(valid_lcid(lcid))
    if ((uecri != 0) && (slices_map.find(uecri) == slices_map.end())){
      slices_map[uecri] = slice_id;
    }
		if (slice_id == 0){
			for(int slice = 1; slice < NUM_SLICES; ++slice){
				rrc[slice]->set_state(3);
				rrc[slice]->set_sib1(rrc[0]->get_sib1());
			}
		}
    slices[lcid][slice_id].write_sdu(sdu);
}

void pdcp::add_bearer(int slice_id, uint32_t lcid, LIBLTE_RRC_PDCP_CONFIG_STRUCT *cnfg)
{
  if(lcid < 0 || lcid >= SRSLTE_N_RADIO_BEARERS) {
    pdcp_log->error("Radio bearer id must be in [0:%d] - %d\n", SRSLTE_N_RADIO_BEARERS, lcid);
    return;
  }
  // gines -- configuramos el bearer por defecto a todos los slices
	if (lcid == 1){
  	for (int slice = 0; slice < NUM_SLICES; ++slice){
    	if (!slices[lcid][slice].is_active()) {
      	slices[lcid][slice].init(rlc, rrc[slice], gw[slice], pdcp_log, lcid, direction, cnfg);
      	pdcp_log->info("Added bearer %s slice %i\n", rb_id_text[lcid], slice);
    	} else {
      	pdcp_log->warning("Bearer %s already configured on slice %i. Reconfiguration not supported\n", rb_id_text[lcid],slice);
    	}
  	}
	}
	else{
    if (!slices[lcid][slice_id].is_active()) {
      slices[lcid][slice_id].init(rlc, rrc[slice_id], gw[slice_id], pdcp_log, lcid, direction, cnfg);
    	pdcp_log->info("Added bearer %s slice %i\n", rb_id_text[lcid], slice_id);
    } else {
      pdcp_log->warning("Bearer %s already configured on slice %i. Reconfiguration not supported\n", rb_id_text[lcid],slice_id);
    }
	}
}

void pdcp::config_security(int slice_id,
                           uint32_t lcid,
                           uint8_t *k_rrc_enc,
                           uint8_t *k_rrc_int,
                           CIPHERING_ALGORITHM_ID_ENUM cipher_algo,
                           INTEGRITY_ALGORITHM_ID_ENUM integ_algo)
{
  if(valid_lcid(lcid))
    slices[lcid][slice_id].config_security(k_rrc_enc, k_rrc_int, cipher_algo, integ_algo);
}

int pdcp::get_current_slice(){
  return current_slice;
}

/* Performs needed configurations to allow multiple NAS protocol executions
	 	- slice_id: identifier of the slice already configured
	  - ip_addr:	array of integers with 4 positions.

		# Configurations:
			- Store the IP address assigned to a specific slice
			- Update the slice being configured (current_slice)
			- Starts configuration process for the next slice if there are remaining slices
			- Otherwise enables data transfer in PDCP
*/
void pdcp::slice_configured(int slice_id, uint8 ip_addr[]){

	std::string str_ip;
	std::stringstream sstm;
	sstm << static_cast<int>(ip_addr[0]) << "." << static_cast<int>(ip_addr[1]) << "." << static_cast<int>(ip_addr[2]) << "." << static_cast<int>(ip_addr[3]);
	str_ip = sstm.str();
  dst_ip_to_slice[str_ip] = current_slice;  

  current_slice++;

	// Start NAS procedure for the next slice
	if (current_slice < NUM_SLICES)
	{ 
  	write_pdu(0, current_slice, connection_setup_pdu);
	}	
	else if (current_slice == NUM_SLICES)
	{
		data_ready = true;	
	}
}

void pdcp::store_con_setup(byte_buffer_t *pdu){
	connection_setup_pdu = pdu;
}

/*******************************************************************************
  RLC interface
*******************************************************************************/
void pdcp::write_pdu(uint32_t lcid, uint64_t slice_id, byte_buffer_t *pdu)
{
  if(valid_lcid(lcid))
  {
		if (!data_ready)
		{
    	slices[lcid][current_slice].write_pdu(pdu);
		}
		else if (data_ready)
		{
			std::string str_dst_ip;
  		std::stringstream sstm;
  		sstm << static_cast<int>(pdu->msg[18]) << "." << static_cast<int>(pdu->msg[19]) << "." << static_cast<int>(pdu->msg[20]) << "." << static_cast<int>(pdu->msg[21]);
  		str_dst_ip = sstm.str();
			slices[lcid][dst_ip_to_slice[str_dst_ip]].write_pdu(pdu);
		}
		//TODO: Eliminar el uso de las variables SLICE_POINTER y SLICE2
	}
}

void pdcp::write_pdu_bcch_bch(byte_buffer_t *sdu)
{
  // Initial slice will be in charge of perform RA process
  rrc[0]->write_pdu_bcch_bch(sdu);
}

void pdcp::write_pdu_bcch_dlsch(byte_buffer_t *sdu)
{
  rrc[current_slice]->write_pdu_bcch_dlsch(sdu);
}

void pdcp::write_pdu_pcch(byte_buffer_t *sdu)
{
	//TODO: comprobar si este metodo solo es usable por el RRC principal o por todos.
  /*if (SLICE2 == 0){
    rrc[SLICE2]->write_pdu_bcch_dlsch(sdu);
    SLICE2++;
  }
  else{
    rrc[SLICE2]->write_pdu_bcch_dlsch(sdu);
    SLICE2 = 0;
  }*/

  rrc[0]->write_pdu_pcch(sdu);
}

/*******************************************************************************
  Helpers
*******************************************************************************/
bool pdcp::valid_lcid(uint32_t lcid)
{
  if(lcid < 0 || lcid >= SRSLTE_N_RADIO_BEARERS) {
    pdcp_log->error("Radio bearer id must be in [0:%d] - %d", SRSLTE_N_RADIO_BEARERS, lcid);
    return false;
  }
  if(!slices[lcid][0].is_active()) {
    pdcp_log->error("PDCP entity for logical channel %d has not been activated\n", lcid);
    return false;
  }
  return true;
}

} // namespace srsue
