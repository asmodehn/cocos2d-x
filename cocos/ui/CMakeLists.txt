ADD_DEFINITIONS (-D_USEGUIDLL)

set(COCOS_UI_SRC
  CCProtectedNode.cpp
  CocosGUI.cpp
  UIButton.cpp
  UICheckBox.cpp
  UIHBox.cpp
  UIHelper.cpp
  UIImageView.cpp
  UILayout.cpp
  UILayoutParameter.cpp
  UILayoutManager.cpp
  UIListView.cpp
  UILoadingBar.cpp
  UIPageView.cpp
  UIRelativeBox.cpp
  UIRichText.cpp
  UIScrollView.cpp
  UISlider.cpp
  UITextAtlas.cpp
  UITextBMFont.cpp
  UIText.cpp
  UITextField.cpp
  UIVBox.cpp
  UIWidget.cpp
  UIDeprecated.cpp
  UIScale9Sprite.cpp
)

include_directories(
  ..
)

add_library(cocosui SHARED
  ${COCOS_UI_SRC}
)

target_link_libraries(cocosui
  cocos2d
)

set_target_properties(cocosui
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)
