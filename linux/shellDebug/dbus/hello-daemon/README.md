# The IR Service Daemon
1. Launch the ir daemon
    /ir-daemon

# To check the DBus interface: fs1
dbus-send --system --print-reply --dest=org.freedesktop.DBus /org/freedesktop/DBus org.freedesktop.DBus.ListNames | grep fs1

dbus-send --system --dest=com.harman.video.fs1 --type=method_call --print-reply /com/harman/video/fs1/ir org.freedesktop.DBus.Introspectable.Introspect


# The D-Bus commands for pass through function
dbus-send --system --type=signal /com/harman/video/fs1/ir com.harman.video.fs1.DataChanged string:test dict:string:string:"irEnablePassThrough","1"

dbus-send --system --type=signal /com/harman/video/fs1/ir com.harman.video.fs1.DataChanged string:test dict:string:string:"irPassThroughPeerIP","192.168.1.104"
