set(FMOD_LIBRARY_DIR ${CMAKE_CURRENT_LIST_DIR}/lib)

add_library( fmod SHARED IMPORTED )

if(${CMAKE_SIZEOF_VOID_P} EQUAL "8")
  set(ARCH "x86_64")
else()
  set(ARCH "x86")  
endif()
  
if(MSVC)
  set_target_properties( fmod PROPERTIES IMPORTED_LOCATION  "${FMOD_LIBRARY_DIR}/win/${ARCH}/")
  set_target_properties( fmod PROPERTIES IMPORTED_IMPLIB "${FMOD_LIBRARY_DIR}/win/${ARCH}/fmod_vc.lib")
  set(FMOD_DLLS ${FMOD_LIBRARY_DIR}/win/${ARCH}/fmod.dll)
elseif((CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX) AND NOT APPLE)
  set_target_properties( fmod PROPERTIES IMPORTED_LOCATION "${FMOD_LIBRARY_DIR}/linux/${ARCH}/libfmod.so.7")
  set_target_properties( fmod PROPERTIES IMPORTED_IMPLIB "${FMOD_LIBRARY_DIR}/linux/${ARCH}/libfmod.so.7")
  set(FMOD_DLLS ${FMOD_LIBRARY_DIR}/linux/${ARCH}/libfmod.so.7)
endif()

set(FMOD_LIBRARIES fmod)
