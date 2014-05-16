import QtQuick 2.2
import QtQuick.Window 2.1
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.1
import gunoodaddy 1.0

Rectangle {

	InputMethodSelectWindow {
		id: inputMethodWindow

		onActionAdded: {
			lazybones.currentPreset.addAction(inputMethodWindow.inputAction)
		}

		onActionUpdated: {

		}
	}

	Column {
		id: topLayer
		anchors { top: parent.top; left: parent.left; right: parent.right; }
		spacing: 5

		Row {
			spacing: 3
			ComboBox {
				id: comboPreset
				width: 200
				model: lazybones.presetList
				editable: true
				focus: true
				onCurrentIndexChanged: {
					lazybones.currentPresetIndex = currentIndex
				}
			}
			Button {
				focus: true
				text: "New"
				onClicked: {
					lazybones.addNewPreset(comboPreset.editText);
					comboPreset.currentIndex = comboPreset.count - 1
				}
			}
			Button {
				focus: true
				text: "Save"
				onClicked: {
					if(comboPreset.editText.length > 0) {
						lazybones.currentPreset.title = comboPreset.editText
					}
				}
			}
		}

		Row {
			width: parent.width
			Button {
				enabled: !lazybones.currentPreset.running
				width: parent.width / 3
				text: "Add Input Method"
				onClicked: {
					inputMethodWindow.inputAction = null
					gc()
					inputMethodWindow.show()
				}
			}

			Button {
				enabled: !lazybones.currentPreset.running
				width: parent.width / 3
				text: lazybones.currentPreset.recording ? "Record Stop" : "Record Start"
				onClicked: {
					lazybones.currentPreset.recording = !lazybones.currentPreset.recording;
				}
			}

			Button {
				enabled: !lazybones.currentPreset.running
				width: parent.width / 3
				text: "Clear"
				onClicked: {
					lazybones.currentPreset.clear()
				}
			}
		}

		Row {
			width: parent.width
			height: 30
			spacing: 10

			Button {
				width: parent.width / 3
				height: parent.height
				anchors.verticalCenter: parent.verticalCenter
				text: lazybones.currentPreset.running ? "Stop" : "Start"
				onClicked: {
					lazybones.currentPreset.running = !lazybones.currentPreset.running
				}
			}

			Text {
				font.bold: true
				font.pixelSize: 17
				anchors.verticalCenter: parent.verticalCenter
				text: {
					var cntStr = (lazybones.currentPreset.currentIndex + 1) + " / " + lazybones.currentPreset.count();
					var percent = lazybones.currentPreset.count() > 0 ? Math.round(((lazybones.currentPreset.currentIndex + 1) * 100 / lazybones.currentPreset.count())) : 0
					return cntStr + " (" + percent + "%)"
				}
			}
		}
	}

	ScrollView {
		id: actionList
		anchors.top: topLayer.bottom
		anchors.topMargin: 5
		anchors.bottom: parent.bottom
		anchors.left: parent.left
		anchors.right: parent.right

		flickableItem.interactive: true
		frameVisible: true
		ListView {
			id: listView
			anchors.fill: parent
			//flexibleCacheBuffer: false
			clip: true
			//atYEnd: true

			onContentHeightChanged: {
				if(!lazybones.currentPreset.recording)
					return;
				var tempContentY = contentHeight - height
				if(tempContentY < 0)
					tempContentY = 0;
				contentY = tempContentY;
			}

			model: lazybones.currentPreset.model
			delegate: Rectangle {
				width: parent.width
				height: 25

				color: {
					if(itemData.actionType === InputAction.KeyPress)
						return "PaleGreen";
					if(itemData.actionType === InputAction.MouseLeftClick)
						return "Orange";
					if(itemData.actionType === InputAction.MouseLeftPress)
						return "#F75D59";
					if(itemData.actionType === InputAction.MouseLeftRelease)
						return "Coral";
					if(itemData.actionType === InputAction.MouseRightClick)
						return "LightSkyBlue";
					if(itemData.actionType === InputAction.MouseRightPress)
						return "LightCyan";
					if(itemData.actionType === InputAction.MouseRightRelease)
						return "Lavender";
					return "white"
				}

				// Running mark
				Text {
					id: noText
					anchors.left: parent.left
					anchors.leftMargin: 5
					anchors.verticalCenter: parent.verticalCenter
					text: (index + 1) + ":"
				}

				// Running mark
				Text {
					id: statusMark
					anchors.left: noText.right
					anchors.leftMargin: 5
					anchors.verticalCenter: parent.verticalCenter
					text: itemData !== undefined ? (itemData.running ? "▶" : "▷") : ""
				}

				// Action Title
				Text {
					anchors.left: statusMark.right
					anchors.leftMargin: 5
					anchors.verticalCenter: parent.verticalCenter

					text: {
						var str = itemData.description
						return str;
					}
				}

				// Delete Button
				Rectangle {
					id: delButton
					radius: 3
					width: 60
					height: parent.height - 4
					border.width: 1
					border.color: "#BBBDBF"
					anchors.right: parent.right
					anchors.rightMargin: 5
					anchors.verticalCenter: parent.verticalCenter

					Text {
						text: "Del"
						anchors.centerIn: parent
					}

					MouseArea {
						anchors.fill: parent
						onClicked: {
							lazybones.currentPreset.removeAction(index)
						}
					}
				}

				// Update Button
				Rectangle {
					radius: 3
					width: 60
					height: parent.height - 4
					border.width: 1
					border.color: "#BBBDBF"
					anchors.right: delButton.left
					anchors.rightMargin: 5
					anchors.verticalCenter: parent.verticalCenter

					Text {
						text: "Update"
						anchors.centerIn: parent
					}

					MouseArea {
						anchors.fill: parent
						onClicked: {
							inputMethodWindow.inputAction = null
							inputMethodWindow.inputAction = itemData
							inputMethodWindow.show()
						}
					}
				}

				// Bottom Line
				Rectangle {
					color: "#A0A0A0"
					width: parent.width
					height: 1
					anchors.bottom: parent.bottom
				}
			}
		}
	}
}
