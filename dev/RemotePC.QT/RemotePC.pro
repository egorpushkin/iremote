# -------------------------------------------------
# RemotePC Project File
# -------------------------------------------------
TARGET = RemotePC
TEMPLATE = app
INCLUDEPATH = ../3rdParty/boost \
    ../3rdParty/cmnkit/source/include \
    ../3rdParty/cmnkit/source/sdk/include \
    ../3rdParty/Expat \
    ../3rdParty/pugi \
    ../3rdParty/hashlib++ \
    ../3rdParty/libjpeg \
    ../iRemote.Cpp \
    ./UI

clang {
    # Disable some harmless warnings
    QMAKE_CXXFLAGS_WARN_ON += -Wno-parentheses-equality
    QMAKE_CXXFLAGS_WARN_ON += -Wno-unused-parameter
    QMAKE_CXXFLAGS_WARN_ON += -Wno-unused-private-field
}

QT += widgets
win32 {
    LIBS += libws2_32 \
        libmswsock \
        libuser32
    DEFINES += REMOTEPC_OS_WINVISTA
    HEADERS += Hardware/Win32/DisplayControl.h \
        Hardware/Win32/HardwareControl.h \
        Hardware/Win32/IVolumeControlInitializer.h \
        Hardware/Vista/EndpointVolumeControl.h
    SOURCES += Hardware/Win32/DisplayControl.cpp \
        Hardware/Win32/HardwareControl.cpp \
        Hardware/Vista/EndpointVolumeControl.cpp
    FORMS += Resources/Win/settingsdialog.ui \
        Resources/Win/dialog.ui \
        Resources/Win/authdialog.ui \
        Resources/Win/aboutdialog.ui \
        Resources/Win/edituserdialog.ui \
        Resources/Win/rememberuserdialog.ui \
        Resources/Win/usersmngdialog.ui
}
macx {
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.6
    CONFIG(release, debug|release) {
        LIBS += ../3rdParty/cmnkit/source/lib/macx/release/libMinCOM.a
        PRE_TARGETDEPS = ../3rdParty/cmnkit/source/lib/macx/release/libMinCOM.a
        OBJECTS_DIR = ./release
        DESTDIR = ../bin/macx/release
    }
    CONFIG(debug, debug|release) {
        LIBS += ../3rdParty/cmnkit/source/lib/macx/debug/libMinCOM.a
        PRE_TARGETDEPS = ../3rdParty/cmnkit/source/lib/macx/debug/libMinCOM.a
        OBJECTS_DIR = ./debug
        DESTDIR = ../bin/macx/debug
    }
    HEADERS += Hardware/Mac/HardwareControl.h \
        Hardware/Mac/VolumeControl.h \
        Hardware/Mac/DisplayControl.h \
        Hardware/Mac/KeyboardLeds.h \
        Hardware/Mac/KeyboardEvents.h \
        Hardware/Mac/AppNapSuspender.h \
        Server/Shell/Processes.bsd.h
    OBJECTIVE_SOURCES += Hardware/Mac/HardwareControl.mm \
        Hardware/Mac/VolumeControl.mm \
        Hardware/Mac/DisplayControl.mm \
        Hardware/Mac/KeyboardLeds.cpp \
        Hardware/Mac/KeyboardEvents.mm \
        Hardware/Mac/AppNapSuspender.mm \
        ../iRemote.Cpp/SFB/Server/ImageSource.mm
    SOURCES += Server/Shell/Processes.bsd.cpp
    ICON = Resources/Mac/RemotePC.icns
    QMAKE_INFO_PLIST = Resources/Mac/Info.plist
    FORMS += Resources/Mac/settingsdialog.ui \
        Resources/Mac/dialog.ui \
        Resources/Mac/authdialog.ui \
        Resources/Mac/aboutdialog.ui \
        Resources/Mac/edituserdialog.ui \
        Resources/Mac/rememberuserdialog.ui \
        Resources/Mac/usersmngdialog.ui
    LIBS += \
        -framework Foundation \
        -framework CoreAudio \
        -framework DiscRecording \
        -framework IOKit \
        -framework AppKit \
        -framework OpenGL
}
HEADERS += \
    Common/Common.h \
    Hardware/Win32/HardwareControl.h \
    Server/ServerContext.h \
    Server/NetworkHelper.h \
    Server/IUIEvents.h \
    Server/IServerControl.h \
    Server/Config.h \
    Server/Client.h \
    Server/AuthContext.h \
    UI/UIConfig.h \
    UI/settingsdialog.h \
    UI/dialog.h \
    UI/authdialog.h \
    UI/aboutdialog.h \
    Hardware/Provider/IVolumeControl.h \
    Hardware/Provider/IMouseControl.h \
    Hardware/Provider/IKeyboardControl.h \
    Hardware/Provider/HardwareProvider.h \
    Server/UsersManager/UsersManager.h \
    Server/UsersManager/User.h \
    Server/UsersManager/DUsersManagerStub.h \
    Server/UsersManager/DUsersManager.h \
    UI/rememberuserdialog.h \
    UI/edituserdialog.h \
    UI/usersmngdialog.h \
    Server/Shell/ShellSupport.h \
    Server/Shell/IShellControl.h \
    Server/Shell/ShellContext.h \
    Server/Shell/ShellHost.h \
    Server/Shell/ProcessTool.h \
    Server/Shell/CtrlHandler.h \
    Hardware/Mac/VirtualKeysMapper.h \
    Server/IClientHandler.h \
    Hardware/Shared/ScreenshotSpreader.h \
    Common/InstanceLock.h \
    Server/OldiRemoteContext.h \
    UI/Controls.h \
    Hardware/Provider/IAudioInput.h \
    Hardware/Hardware.h \
    UI/Annunciators/Events.h \
    UI/Annunciators/UsersEvents.h \
    Hardware/Provider/IDisplayControl.h \
    ../3rdParty/pugi/pugixml.hpp \
    ../3rdParty/pugi/pugiconfig.hpp \
    ../3rdParty/libjpeg/turbojpeg.h \
    ../3rdParty/libjpeg/transupp.h \
    ../3rdParty/libjpeg/rrutil.h \
    ../3rdParty/libjpeg/rrtimer.h \
    ../3rdParty/libjpeg/jversion.h \
    ../3rdParty/libjpeg/jsimddct.h \
    ../3rdParty/libjpeg/jsimd.h \
    ../3rdParty/libjpeg/jpeglib.h \
    ../3rdParty/libjpeg/jpegint.h \
    ../3rdParty/libjpeg/jmorecfg.h \
    ../3rdParty/libjpeg/jmemsys.h \
    ../3rdParty/libjpeg/jinclude.h \
    ../3rdParty/libjpeg/jerror.h \
    ../3rdParty/libjpeg/jdhuff.h \
    ../3rdParty/libjpeg/jdct.h \
    ../3rdParty/libjpeg/jconfig.h.in \
    ../3rdParty/libjpeg/jchuff.h \
    ../3rdParty/libjpeg/cdjpeg.h \
    ../3rdParty/libjpeg/cderror.h \
    ../3rdParty/libjpeg/bmp.h \
    ../3rdParty/libjpeg/jconfig.h \
    Server/Services/IServicesManager.h \
    Server/Services/ServicesManager.h \
    Server/Services/IServiceHandler.h \
    Server/Services/ServiceDescriptor.h \
    Server/Services/IDevice.h \
    Server/Services/Device.h \
    Server/Services/Services.h \
    Server/Services/IDeviceHandler.h \
    ../iRemote.Cpp/KeyCodes/VirtualCodes.Win32.h \
    ../iRemote.Cpp/Messages/ZoomParamsMessage.h \
    ../iRemote.Cpp/Messages/ZoomLevelMessage.h \
    ../iRemote.Cpp/Messages/VolumeMessage.h \
    ../iRemote.Cpp/Messages/StringMessage.h \
    ../iRemote.Cpp/Messages/StreamRequest.h \
    ../iRemote.Cpp/Messages/ShellResponse.h \
    ../iRemote.Cpp/Messages/ShellRequest.h \
    ../iRemote.Cpp/Messages/ScrollMessage2.h \
    ../iRemote.Cpp/Messages/ScrollMessage.h \
    ../iRemote.Cpp/Messages/ScreenshotMessage.h \
    ../iRemote.Cpp/Messages/PasswordRequest.h \
    ../iRemote.Cpp/Messages/PasswordMessage.h \
    ../iRemote.Cpp/Messages/MoveToMessage.h \
    ../iRemote.Cpp/Messages/MoveMessage.h \
    ../iRemote.Cpp/Messages/MouseClickMessage.h \
    ../iRemote.Cpp/Messages/Messages.h \
    ../iRemote.Cpp/Messages/KeyboardMessage.h \
    ../iRemote.Cpp/Messages/IStringMessage.h \
    ../iRemote.Cpp/Messages/IScreenshotMessage.h \
    ../iRemote.Cpp/Messages/IDataMessage.h \
    ../iRemote.Cpp/Messages/DataMessage.h \
    ../iRemote.Cpp/Messages/CookieMessage.h \
    ../iRemote.Cpp/Messages/AuthResponse2.h \
    ../iRemote.Cpp/Messages/AuthResponse.h \
    ../iRemote.Cpp/Messages/AuthMessage.h \
    ../iRemote.Cpp/Messages/AudioMessages.h \
    ../iRemote.Cpp/Versions/Versions.h \
    ../iRemote.Cpp/Versions/Features.h \
    ../iRemote.Cpp/SFB/Messages/RequestDevice.h \
    ../iRemote.Cpp/SFB/Messages/InitMessage.h \
    ../iRemote.Cpp/SFB/Messages/DeviceNotFound.h \
    ../iRemote.Cpp/SFB/Messages/DeviceInfo.h \
    ../iRemote.Cpp/SFB/Messages/DeviceDisconnected.h \
    ../iRemote.Cpp/SFB/Messages/ControlMessage.h \
    ../iRemote.Cpp/SFB/Messages/BlockMessage.h \
    ../iRemote.Cpp/SFB/Messages/BatchHeader.h \
    ../iRemote.Cpp/SFB/Server/SwapBuffer.h \
    ../iRemote.Cpp/SFB/Server/SFB.h \
    ../iRemote.Cpp/SFB/Server/RawImage.h \
    ../iRemote.Cpp/SFB/Server/RasterFormat.h \
    ../iRemote.Cpp/SFB/Server/MemoryCache.h \
    ../iRemote.Cpp/SFB/Server/MatrixComparator.h \
    ../iRemote.Cpp/SFB/Server/Matrix.h \
    ../iRemote.Cpp/SFB/Server/Math.h \
    ../iRemote.Cpp/SFB/Server/JpegCompressor.h \
    ../iRemote.Cpp/SFB/Server/InitHandler.h \
    ../iRemote.Cpp/SFB/Server/ImageSource.h \
    ../iRemote.Cpp/SFB/Server/FBService.h \
    ../iRemote.Cpp/SFB/Server/FBManager.h \
    ../iRemote.Cpp/SFB/Server/DeliveryQueue.h \
    ../iRemote.Cpp/SFB/Server/DeliveryHandler.h \
    ../iRemote.Cpp/SFB/Server/ByteBuffer.h \
    ../iRemote.Cpp/Messages/ServiceRequest.h
SOURCES += \
    Common/main.cpp \
    Common/Common.cpp \
    ../3rdParty/Expat/xmltok_ns.c \
    ../3rdParty/Expat/xmltok_impl.c \
    ../3rdParty/Expat/xmltok.c \
    ../3rdParty/Expat/xmlrole.c \
    ../3rdParty/Expat/xmlparse.c \
    Server/ServerContext.cpp \
    Server/NetworkHelper.cpp \
    Server/Config.cpp \
    Server/Client.cpp \
    Server/AuthContext.cpp \
    Hardware/Provider/HardwareProvider.cpp \
    Hardware/Provider/HardwareGuids.cpp \
    ../3rdParty/hashlib++/hl_sha512wrapper.cpp \
    ../3rdParty/hashlib++/hl_sha384wrapper.cpp \
    ../3rdParty/hashlib++/hl_sha256wrapper.cpp \
    ../3rdParty/hashlib++/hl_sha256.cpp \
    ../3rdParty/hashlib++/hl_sha2ext.cpp \
    ../3rdParty/hashlib++/hl_sha1wrapper.cpp \
    ../3rdParty/hashlib++/hl_sha1.cpp \
    ../3rdParty/hashlib++/hl_md5wrapper.cpp \
    ../3rdParty/hashlib++/hl_md5.cpp \
    Server/UsersManager/UsersManager.cpp \
    Server/UsersManager/User.cpp \
    Server/UsersManager/DUsersManagerStub.cpp \
    Server/Shell/ShellContext.cpp \
    Server/Shell/ShellHost.cpp \
    Server/Shell/ProcessTool.cpp \
    Server/Shell/CtrlHandler.cpp \
    Hardware/Mac/VirtualKeysMapper.cpp \
    Hardware/Shared/ScreenshotSpreader.cpp \
    Common/InstanceLock.cpp \
    Server/OldiRemoteContext.cpp \
    ../3rdParty/pugi/pugixml.cpp \
    ../3rdParty/libjpeg/bmp.c \
    ../3rdParty/libjpeg/wrtarga.c \
    ../3rdParty/libjpeg/wrppm.c \
    ../3rdParty/libjpeg/wrgif.c \
    ../3rdParty/libjpeg/wrbmp.c \
    ../3rdParty/libjpeg/turbojpegl.c \
    ../3rdParty/libjpeg/transupp.c \
    ../3rdParty/libjpeg/rdtarga.c \
    ../3rdParty/libjpeg/rdswitch.c \
    ../3rdParty/libjpeg/rdppm.c \
    ../3rdParty/libjpeg/rdgif.c \
    ../3rdParty/libjpeg/rdcolmap.c \
    ../3rdParty/libjpeg/rdbmp.c \
    ../3rdParty/libjpeg/jutils.c \
    ../3rdParty/libjpeg/jsimd_none.c \
    ../3rdParty/libjpeg/jquant2.c \
    ../3rdParty/libjpeg/jquant1.c \
    ../3rdParty/libjpeg/jmemnobs.c \
    ../3rdParty/libjpeg/jmemmgr.c \
    ../3rdParty/libjpeg/jidctred.c \
    ../3rdParty/libjpeg/jidctint.c \
    ../3rdParty/libjpeg/jidctfst.c \
    ../3rdParty/libjpeg/jidctflt.c \
    ../3rdParty/libjpeg/jfdctint.c \
    ../3rdParty/libjpeg/jfdctfst.c \
    ../3rdParty/libjpeg/jfdctflt.c \
    ../3rdParty/libjpeg/jerror.c \
    ../3rdParty/libjpeg/jdtrans.c \
    ../3rdParty/libjpeg/jdsample.c \
    ../3rdParty/libjpeg/jdpostct.c \
    ../3rdParty/libjpeg/jdphuff.c \
    ../3rdParty/libjpeg/jdmerge.c \
    ../3rdParty/libjpeg/jdmaster.c \
    ../3rdParty/libjpeg/jdmarker.c \
    ../3rdParty/libjpeg/jdmainct.c \
    ../3rdParty/libjpeg/jdinput.c \
    ../3rdParty/libjpeg/jdhuff.c \
    ../3rdParty/libjpeg/jddctmgr.c \
    ../3rdParty/libjpeg/jdcolor.c \
    ../3rdParty/libjpeg/jdcoefct.c \
    ../3rdParty/libjpeg/jdatasrc.c \
    ../3rdParty/libjpeg/jdatadst.c \
    ../3rdParty/libjpeg/jdapistd.c \
    ../3rdParty/libjpeg/jdapimin.c \
    ../3rdParty/libjpeg/jctrans.c \
    ../3rdParty/libjpeg/jcsample.c \
    ../3rdParty/libjpeg/jcprepct.c \
    ../3rdParty/libjpeg/jcphuff.c \
    ../3rdParty/libjpeg/jcparam.c \
    ../3rdParty/libjpeg/jcomapi.c \
    ../3rdParty/libjpeg/jcmaster.c \
    ../3rdParty/libjpeg/jcmarker.c \
    ../3rdParty/libjpeg/jcmainct.c \
    ../3rdParty/libjpeg/jcinit.c \
    ../3rdParty/libjpeg/jchuff.c \
    ../3rdParty/libjpeg/jcdctmgr.c \
    ../3rdParty/libjpeg/jccolor.c \
    ../3rdParty/libjpeg/jccoefct.c \
    ../3rdParty/libjpeg/jcapistd.c \
    ../3rdParty/libjpeg/jcapimin.c \
    ../3rdParty/libjpeg/cdjpeg.c \
    Server/Services/ServicesManager.cpp \
    Server/Services/ServiceDescriptor.cpp \
    Server/Services/Device.cpp \
    ../iRemote.Cpp/Messages/ZoomParamsMessage.cpp \
    ../iRemote.Cpp/Messages/ZoomLevelMessage.cpp \
    ../iRemote.Cpp/Messages/VolumeMessage.cpp \
    ../iRemote.Cpp/Messages/StringMessage.cpp \
    ../iRemote.Cpp/Messages/StreamRequest.cpp \
    ../iRemote.Cpp/Messages/ShellResponse.cpp \
    ../iRemote.Cpp/Messages/ShellRequest.cpp \
    ../iRemote.Cpp/Messages/ScrollMessage2.cpp \
    ../iRemote.Cpp/Messages/ScrollMessage.cpp \
    ../iRemote.Cpp/Messages/ScreenshotMessage.cpp \
    ../iRemote.Cpp/Messages/PasswordRequest.cpp \
    ../iRemote.Cpp/Messages/PasswordMessage.cpp \
    ../iRemote.Cpp/Messages/MoveToMessage.cpp \
    ../iRemote.Cpp/Messages/MoveMessage.cpp \
    ../iRemote.Cpp/Messages/MouseClickMessage.cpp \
    ../iRemote.Cpp/Messages/Messages.cpp \
    ../iRemote.Cpp/Messages/KeyboardMessage.cpp \
    ../iRemote.Cpp/Messages/DataMessage.cpp \
    ../iRemote.Cpp/Messages/CookieMessage.cpp \
    ../iRemote.Cpp/Messages/AuthResponse2.cpp \
    ../iRemote.Cpp/Messages/AuthResponse.cpp \
    ../iRemote.Cpp/Messages/AuthMessage.cpp \
    ../iRemote.Cpp/Messages/AudioMessages.cpp \
    ../iRemote.Cpp/Versions/Versions.cpp \
    ../iRemote.Cpp/Versions/Features.cpp \
    ../iRemote.Cpp/SFB/Messages/RequestDevice.cpp \
    ../iRemote.Cpp/SFB/Messages/InitMessage.cpp \
    ../iRemote.Cpp/SFB/Messages/DeviceNotFound.cpp \
    ../iRemote.Cpp/SFB/Messages/DeviceInfo.cpp \
    ../iRemote.Cpp/SFB/Messages/DeviceDisconnected.cpp \
    ../iRemote.Cpp/SFB/Messages/ControlMessage.cpp \
    ../iRemote.Cpp/SFB/Messages/BlockMessage.cpp \
    ../iRemote.Cpp/SFB/Messages/BatchHeader.cpp \
    ../iRemote.Cpp/SFB/Server/SwapBuffer.cpp \
    ../iRemote.Cpp/SFB/Server/RawImage.cpp \
    ../iRemote.Cpp/SFB/Server/RasterFormat.cpp \
    ../iRemote.Cpp/SFB/Server/MemoryCache.cpp \
    ../iRemote.Cpp/SFB/Server/MatrixComparator.cpp \
    ../iRemote.Cpp/SFB/Server/Matrix.cpp \
    ../iRemote.Cpp/SFB/Server/JpegCompressor.cpp \
    ../iRemote.Cpp/SFB/Server/InitHandler.cpp \
    ../iRemote.Cpp/SFB/Server/FBService.cpp \
    ../iRemote.Cpp/SFB/Server/FBManager.cpp \
    ../iRemote.Cpp/SFB/Server/DeliveryQueue.cpp \
    ../iRemote.Cpp/SFB/Server/DeliveryHandler.cpp \
    ../iRemote.Cpp/SFB/Server/ByteBuffer.cpp \
    UI/usersmngdialog.cpp \
    UI/UIConfig.cpp \
    UI/settingsdialog.cpp \
    UI/rememberuserdialog.cpp \
    UI/edituserdialog.cpp \
    UI/dialog.cpp \
    UI/Controls.cpp \
    UI/authdialog.cpp \
    UI/aboutdialog.cpp \
    UI/Annunciators/UsersEvents.cpp \
    UI/Annunciators/Events.cpp \
    ../iRemote.Cpp/Messages/ServiceRequest.cpp
RESOURCES += RemotePC.qrc
OTHER_FILES += Resources/RemotePC.icns \
    Resources/Mac/RemotePC.icns \
    Resources/Mac/Info.plist \
    ../3rdParty/libjpeg/jconfig.doc \
    ../3rdParty/libjpeg/config.h.in

