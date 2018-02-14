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

#ifndef PDCP_H
#define PDCP_H

#include "srslte/common/log.h"
#include "srslte/common/common.h"
#include "srslte/interfaces/ue_interfaces.h"
#include "srslte/upper/pdcp_entity.h"

#include <iostream>
#include <map>
#include <string>
using namespace std;

namespace srslte {

class pdcp
    :public srsue::pdcp_interface_gw
    ,public srsue::pdcp_interface_rlc
    ,public srsue::pdcp_interface_rrc
{
public:
  pdcp();
  virtual ~pdcp(){}
  void init(srsue::rlc_interface_pdcp *rlc_,
            srsue::rrc_interface_pdcp *rrc_[],
            srsue::gw_interface_pdcp *gw_[],
            log *pdcp_log_,
            uint8_t direction_);
  void stop();

  // RRC interface
  void reset();
  void write_sdu(int slice_identifier, uint32_t lcid, byte_buffer_t *sdu);
	void write_sdu_and_next_slice(int slice_id, uint32_t lcid, byte_buffer_t *sdu);
  void write_sdu_uecri(int slice_identifier, uint64_t uecri, uint32_t lcid, byte_buffer_t *sdu);
  void add_bearer(int slice_identifier, uint32_t lcid, LIBLTE_RRC_PDCP_CONFIG_STRUCT *cnfg = NULL);
  void config_security(int slice_identifier,
                       uint32_t lcid,
                       uint8_t *k_rrc_enc,
                       uint8_t *k_rrc_int,
                       CIPHERING_ALGORITHM_ID_ENUM cipher_algo,
                       INTEGRITY_ALGORITHM_ID_ENUM integ_algo);

  // RLC interface
  void write_pdu(uint32_t lcid, uint64_t slice_id, byte_buffer_t *sdu);
  void write_pdu_bcch_bch(byte_buffer_t *sdu);
  void write_pdu_bcch_dlsch(byte_buffer_t *sdu);
  void write_pdu_pcch(byte_buffer_t *sdu);

  //gines
  int  get_current_slice();
  void slice_configured(int slice_identifier, uint8 addr[]);
	void store_con_setup(byte_buffer_t *pdu);

  const static int 	NUM_SLICES = 2;
	byte_buffer_t			*connection_setup_pdu; 
  int  	current_slice;
  bool 	data_ready;
  int SLICE2;

  std::map<uint64_t,int> slices_map;
	std::map<string,int>   dst_ip_to_slice;

private:
  log                              *pdcp_log;
  //pdcp_entity                      pdcp_array[SRSLTE_N_RADIO_BEARERS];
  pdcp_entity                    slices[SRSLTE_N_RADIO_BEARERS][NUM_SLICES];

  srsue::rlc_interface_pdcp        *rlc;
  srsue::rrc_interface_pdcp        *rrc[2];
  srsue::gw_interface_pdcp         *gw[2];

  uint8_t             direction;

  bool valid_lcid(uint32_t lcid);
};

} // namespace srsue


#endif // PDCP_H
