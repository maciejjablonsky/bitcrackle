import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Bitcrackle

Rectangle {
    color: Colors.background
    width: 1024
    height: 680

    Column {
        anchors.fill: parent
        anchors.margins: Sizing.l
        spacing: Sizing.m

        RowLayout {
            id: menuBar
            height: childrenRect.height
            anchors.left: parent.left
            anchors.right: parent.right

            Rectangle {
                id: devicePicker
                radius: Sizing.s
                color: Colors.surface

                height: Sizing.l * 2
                width: childrenRect.width

                Text {
                    text: 'DEVICE'
                    font.family: 'arial'
                    font.pointSize: 12
                    font.bold: true
                    color: Colors.primary
                    anchors.verticalCenter: parent.verticalCenter
                }
            }
        }

        Rectangle {
            color: Colors.surface
            anchors.left: parent.left
            anchors.right: parent.right
            height: Sizing.l * 6
            radius: Sizing.s


        }

        // Rectangle {
        //     color: 'red'
        //     anchors.left: parent.left
        //     anchors.right: parent.right
        //     width: 10
        //     height: 10
        // }10
    }

    // Rectangle {
	// width: 20
	// height: 20
	// 	//width: childrenRect.width
	// 	//height: childrenRect.height
    // }
    // ColumnLayout {
    //
    //     anchors.centerIn: parent
    //     spacing: 20
    //
    //     ComboBox {
	// 		model: midiDeviceModel
	// 		textRole: "deviceName"
	// 		currentIndex: midiDeviceModel.currentIndex
    //
	// 		onCurrentIndexChanged: {
	// 			midiDeviceModel.set_current_device(modelData) // or however you want to sync
	// 		}
	// 	}
    // }
}
