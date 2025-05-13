import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Bitcrackle

ApplicationWindow {
    visible: true
    title: "MIDI Input Devices"

    Rectangle {
	width: 20
	height: 20
		//width: childrenRect.width
		//height: childrenRect.height
		color: Colors.background
    }
    ColumnLayout {

        anchors.centerIn: parent
        spacing: 20

        // ComboBox {
		// 	model: midiDeviceModel
		// 	textRole: "deviceName"
		// 	currentIndex: midiDeviceModel.currentIndex

		// 	onCurrentIndexChanged: {
		// 		midiDeviceModel.set_current_device(modelData) // or however you want to sync
		// 	}
		// }
    }
}
