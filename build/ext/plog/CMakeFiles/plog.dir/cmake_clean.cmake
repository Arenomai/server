file(REMOVE_RECURSE
  "../../../lib/libplog.pdb"
  "../../../lib/libplog.so"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/plog.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
