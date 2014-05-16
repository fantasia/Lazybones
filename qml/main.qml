import QtQuick 2.2
import QtQuick.Window 2.1
import QtQuick.Controls 1.1
import gunoodaddy 1.0

Window {
	id: mainWindow
	visible: true
	width: 500
	height: 500
	title: "Lazybones ver 1.0 - gunoodaddy 2014"

	Lazybones {
		id: lazybones
	}

	MacroWindow {
		id: macroWindow
		visible: false
	}

	Rectangle {
		anchors.fill: parent
		anchors.margins: 5

		Button {
			id: macroOpenButton
			anchors.top: parent.top
			anchors.topMargin: 10
			anchors.left: parent.left
			anchors.right: parent.right
			text: "Diablo III Macro"
			height: 30
			onClicked: {
				macroWindow.show();
				macroWindow.requestActivate()
				macroWindow.raise()
			}
		}

		GroupBox {
			id: dropBoxPanel
			anchors.top: macroOpenButton.bottom
			anchors.topMargin: 5
			width: parent.width
			title: "DropBox"

			DropBoxPanel {
				width: parent.width
			}
		}

		GroupBox {
			anchors.top: dropBoxPanel.bottom
			anchors.topMargin: 10
			anchors.left: parent.left
			width: parent.width
			height: parent.height - y - 5
			title: "Action Panel"

			ActionPanel {
				anchors.fill: parent
			}
		}
	}
}
