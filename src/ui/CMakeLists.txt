meridian_module(mer_ui
  SOURCES
    AppSettings.cpp
    Theme.cpp
    MonitorView.cpp
    MainWindow.cpp
    TimelineView.cpp
    TimelineRuler.cpp
    BinPanel.cpp
    InspectorPanel.cpp
    ConsolePanel.cpp
    PreferencesDialog.cpp
    TransportBar.cpp
  PUBLIC_DEPS
    Meridian::mer_playback
    Meridian::mer_compat_v1
    Qt6::Widgets)

meridian_link_whole(mer_ui mer_plugins)

target_compile_definitions(mer_ui PRIVATE MERIDIAN_VERSION_STRING="${PROJECT_VERSION}")
