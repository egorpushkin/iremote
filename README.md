# iRemote Suite

iRemote is remote control application for manipulating desktop workstations using mobile device over WiFi. The application allows simulating keyboard and/or mouse input of any kind. Key feature of iRemote Suite is screen sharing capability. 

iRemote Suite features:
 * Desktop screen sharing with zooming support.
 * Image quality is configurable.
 * Mouse control (mouse movement, mouse clicks, tap-to-click, scrolling).
 * Keyboard control.
 * Master volume control.
 * Fully-functional shell access. 
 * Authorization feature. 

Supported desktop platforms:
 * Mac OS X 10.6 or above (Intel only).
 * Windows XP / Windows Vista / Windows 7 / Windows 8.

Supported mobile platforms:
 * iOS 6.0+.
 * Android 2.3+.

Major iRemote releases:

![](https://raw.github.com/egorpushkin/iremote/master/art/Readme/Timeline.png)

This neat story was timed to the submission of Android build to Google Play:  
http://blog.scientific-soft.com/2011/04/08/android-story-of-iremote/

## Internals and Repository Structure

Not much have changed since the moment when this article on iRemote architecture was published. It gives good overview of how desktop and mobile applications are organized.  
http://scientific-soft.com/mobile/iremote/internals

Desktop application is built in QT (http://qt-project.org/). Mobile apps are native.
Among other 3rd Party dependencies (full list can be found here REPO/doc/License/LIBS.txt) there is another project of mine, which actually preceded iRemote. Common Kit library (https://github.com/egorpushkin/common-kit) powers iRemote object model and networking on iOS and desktop platforms. 

This repository contains most materials related to the project:  
&nbsp;&nbsp;&nbsp;&nbsp;/art - Icons, splash screens, etc.  
&nbsp;&nbsp;&nbsp;&nbsp;/doc - Project related documentation, EULA, manuals and some other materials can be found here.  
&nbsp;&nbsp;&nbsp;&nbsp;/dev - Contains mobile and desktop application source and 3rd party dependencies (or references to it).  

You will notice that repository history does not go all the way back to the origins of the project. The repository was cleaned up when the decision was made to make the source public. For the most part cleanup was related to removing resources and demos that could not be published due to licensing limitations. 

## Code Style and Techniques

Not a lot to say here, but still I want to make a note that now I add underscore ("_") in the beginning of member variables and no longer use Camel notation for method names :)

iOS client was shipped prior to background support was introduced in iOS. We could not even dream about ARC and other cool things back then. 

iRemote was ported to Android shortly after Honeycomb was released. In those dark days there were still users asking not to leave 1.5 overboard. This should shed light on some UX-level decisions made at that point.

## Experimental

These are some of iRemote features that were implemented (at least partially) but were not shipped. 

### Smart Frame Buffer

The SFB technology was designed to allow streaming the entire screen (rather than just seeing a portion of it) in an efficient way. Tablet users were supposed to be primary consumers of this technology.  
https://github.com/egorpushkin/iremote/tree/feature/rpc_sfb

### iPad Support in Universal iOS Application

iRemote for iOS was transformed into universal application and iPad UI was prototyped around SFB technology. iPad application is about 90% complete at this point.  
https://github.com/egorpushkin/iremote/tree/feature/ipad

### Media Streaming from Desktop

Simple media streaming server was integrated into Remote PC allowing remotely connected client to browse workstation file system and stream any content supported by native WebView control.

## License 

iRemote source code is available under MIT license.  
http://opensource.org/licenses/MIT

## References

Project Page - http://scientific-soft.com/mobile/iremote  
iOS - https://itunes.apple.com/app/iremote-suite/id312222737?mt=8  
Android - https://play.google.com/store/apps/details?id=com.scientificsoft.iremote.android.iremote  

## About the Author

iRemote project was brought to life by Egor Pushkin. Refer to resources listed below to get a better sense of my activities and interests. In two words my recent efforts are directed towards architecting connected systems (specifically in mobile area) with focus on cross-platform development methodologies, modern communication paradigms and highly automated workflows. 

LinkedIn - https://www.linkedin.com/in/egorpushkin  
Twitter - https://twitter.com/egorpushkin  
Blog - http://blog.scientific-soft.com/  

![](https://raw.github.com/egorpushkin/iremote/master/art/Readme/Progress.png)








