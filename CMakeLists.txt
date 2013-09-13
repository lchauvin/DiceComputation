cmake_minimum_required(VERSION 2.8.9)

#-----------------------------------------------------------------------------
if(NOT Slicer_SOURCE_DIR)
  set(EXTENSION_NAME DiceComputation)
  set(EXTENSION_HOMEPAGE "http://www.slicer.org/slicerWiki/index.php/Documentation/Nightly/Extensions/DiceComputation")
  set(EXTENSION_CATEGORY "Quantification")
  set(EXTENSION_CONTRIBUTORS "Laurent Chauvin (BWH), Sonia Pujol (BWH)")
  set(EXTENSION_DESCRIPTION "Compute Dice's Similarity Coefficient (DSC) for several registered label map images.")
  set(EXTENSION_ICONURL "http://wiki.slicer.org/slicerWiki/images/7/79/DiceSimilarityCoefficient.png")
  set(EXTENSION_SCREENSHOTURLS "http://www.slicer.org/slicerWiki/images/1/1d/Slicer4-DiceComputation-GUI.png")
endif()

#-----------------------------------------------------------------------------
if(NOT Slicer_SOURCE_DIR)
  find_package(Slicer REQUIRED)
  include(${Slicer_USE_FILE})
endif()

#-----------------------------------------------------------------------------
# Add below your modules
add_subdirectory(DiceComputation)

#-----------------------------------------------------------------------------
if(NOT Slicer_SOURCE_DIR)
  include(${Slicer_EXTENSION_CPACK})
endif()