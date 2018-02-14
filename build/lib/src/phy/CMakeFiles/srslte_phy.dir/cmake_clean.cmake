FILE(REMOVE_RECURSE
  "libsrslte_phy.pdb"
  "libsrslte_phy.a"
)

# Per-language clean rules from dependency scanning.
FOREACH(lang)
  INCLUDE(CMakeFiles/srslte_phy.dir/cmake_clean_${lang}.cmake OPTIONAL)
ENDFOREACH(lang)
