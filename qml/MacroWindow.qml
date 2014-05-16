import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Window 2.1
import QtQuick.Dialogs 1.1
import gunoodaddy 1.0

Window {
	id: macroWindow

	width: 550
	height: 500

	property int runningIndex: lazybones.currentMacroPreset.currentIndex
	property int __deletedIndex: -1

	onRunningIndexChanged: {
		if(lazybones.currentMacroPreset.running)
			macroListView.positionViewAtIndex(runningIndex, ListView.Center);
	}

	property GameScreenSettingWindow gameScreenWindow: GameScreenSettingWindow {
		onVisibleChanged: {
			if(!visible) {
				macroWindow.raise()
			}
		}
	}

	property GameScreenListWindow gameScreenListWindow: GameScreenListWindow {
		onVisibleChanged: {
			if(!visible) {
				macroWindow.raise()
			}
		}
	}

	property MacroActionSelectWindow selectWindow: MacroActionSelectWindow {
		onActionAdded: {
			lazybones.currentMacroPreset.insertAction(selectWindow.insertIndex, selectWindow.macroAction)
		}

		onActionUpdated: {
			if(from !== to) {
				lazybones.currentMacroPreset.moveAction(to, action);
			}
		}

		onVisibleChanged: {
			if(!visible) {
				macroWindow.raise()
			}
		}
	}

	property MessageDialog messagePopup: MessageDialog {
		text: "Are you sure you want to delete it?"
		standardButtons: StandardButton.Yes | StandardButton.No
		onYes: {
			if(__deletedIndex >= 0) {
				lazybones.currentMacroPreset.removeAction(__deletedIndex)
			}
		}

	}

	Column {
		id: topLayer
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.top: parent.top
		anchors.leftMargin: 5
		anchors.topMargin: 5
		anchors.rightMargin: 5

		spacing: 5

		Row {
			spacing: 5

			ComboBox {
				id: comboPreset
				width: 250
				model: lazybones.macroPresetList
				editable: true
				onCurrentIndexChanged: {
					lazybones.currentMacroPresetIndex = currentIndex
				}
				onActiveFocusChanged: {
					if(!activeFocus) {
						if(editText.length > 0) {
							lazybones.currentMacroPreset.title = editText
						}
					}
				}
			}
			Button {
				focus: true
				text: "New"
				onClicked: {
					lazybones.addNewMacroPreset(comboPreset.editText);
					comboPreset.currentIndex = comboPreset.count - 1
				}
			}
			Button {
				focus: true
				text: "Save"
				onClicked: {
					if(comboPreset.editText.length > 0) {
						lazybones.currentMacroPreset.title = comboPreset.editText;
						lazybones.save();
					}
				}
			}
		}

		GroupBox {
			title: "Setting"
			width: parent.width
			Column {
				spacing: 5
				Row {
					spacing: 5
					Button {
						text: "Screen Setting"
						onClicked: {
							gameScreenWindow.visible = true
							gameScreenWindow.requestActivate()
						}
					}

					Button {
						text: "Screen List"
						onClicked: {
							gameScreenListWindow.show()
							gameScreenListWindow.requestActivate()
							gameScreenListWindow.raise()
						}
					}
				}

				Row {
					spacing: 5
					TextField {
						id: textWinTitle
						width: 100
						font.pixelSize: 11
						text: lazybones.currentMacroPreset.windowTitle
					}
					TextField {
						id: textWinX
						width: 45
						text: lazybones.currentMacroPreset.windowRect.x
					}
					TextField {
						id: textWinY
						width: 45
						text: lazybones.currentMacroPreset.windowRect.y
					}
					TextField {
						id: textWinWidth
						width: 45
						text: lazybones.currentMacroPreset.windowRect.width
					}
					TextField {
						id: textWinHeight
						width: 45
						text: lazybones.currentMacroPreset.windowRect.height
					}
					Button {
						text: "Set"
						onClicked: {
							lazybones.currentMacroPreset.windowTitle = textWinTitle.text
							lazybones.currentMacroPreset.windowRect = Qt.rect(textWinX.text, textWinY.text, textWinWidth.text, textWinHeight.text);
							lazybones.currentMacroPreset.applyWindowRect();
						}
					}
				}
			}
		}

		Row {
			spacing: 5
			Button {
				text: "Add Action"
				onClicked: {
					selectWindow.insertIndex = macroListView.count
					selectWindow.macroAction = null
					gc()
					selectWindow.visible = true
					selectWindow.requestActivate()
				}
			}

			Button {
				text: lazybones.currentMacroPreset.running ? "Stop" : "Start"
				onClicked: {
					if(!lazybones.currentMacroPreset.running)
						lazybones.currentMacroPreset.clearStatistics();

					lazybones.currentMacroPreset.currentIndex = spinFromStart.value
					lazybones.currentMacroPreset.running = !lazybones.currentMacroPreset.running;
					lazybones.save();
				}
			}

			SpinBox {
				anchors.verticalCenter: parent.verticalCenter
				id: spinFromStart
				value: lazybones.currentMacroPreset.running ? runningIndex : 0
				minimumValue: 0
				maximumValue: macroListView.count - 1
			}

			// Statistics
			Row {
				anchors.verticalCenter: parent.verticalCenter
				spacing: 10
				Text {
					font.weight: Font.Black
					font.pixelSize: 16
					color: "blue"
					style: Text.Raised; styleColor: "black"
					text: "LOOP : " + lazybones.currentMacroPreset.statsLoopCount
				}

				Text {
					font.weight: Font.Black
					font.pixelSize: 16
					color: "red"
					style: Text.Raised; styleColor: "black"
					text: "HANG : " + lazybones.currentMacroPreset.statsHangCount
				}

				Text {
					font.weight: Font.Black
					font.pixelSize: 16
					color: "#FF8000"
					style: Text.Raised; styleColor: "black"
					text: "ITEM : " + lazybones.currentMacroPreset.statsLegendaryCount
				}
			}
		}
	}

	Rectangle {
		anchors.fill: actionList
		visible: macroListView.count <= 0
		Text {
			text: "Empty Macro."
			font.pixelSize: 15
			anchors.centerIn: parent
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
		//frameVisible: true
		ListView {
			id: macroListView
			anchors.fill: parent
			clip: true

			//			onContentHeightChanged: {
			//				var tempContentY = contentHeight - height
			//				if(tempContentY < 0)
			//					tempContentY = 0;
			//				contentY = tempContentY;
			//			}

			model: lazybones.currentMacroPreset.model

			delegate: Rectangle {
				width: parent.width
				height: 25

				color: {
					// http://html-color-codes.info/
					switch(itemData.actionType) {
					case MacroAction.DoInputAction:	return "#E6F8E0";
					case MacroAction.DoInputActionChunk:	return "#CEECF5";
					}

					if(itemData.blockStartType())
						return "#F5A9A9";

					if(itemData.blockEndType())
						return "#F6CECE";
					return "white"
				}

				Text {
					anchors.left: parent.left
					anchors.leftMargin: 5
					anchors.verticalCenter: parent.verticalCenter
					text: index + " : " + ((itemData !== undefined) ? itemData.description : "")
					font.pixelSize: 9
				}

				MouseArea {
					anchors.fill: parent
					onClicked: {
						selectWindow.insertIndex = index
						selectWindow.macroAction = itemData
						selectWindow.show()
					}
				}

				// Delete Button
				Rectangle {
					id: delButton
					radius: width / 2
					width: 22
					height: parent.height - 8
					border.width: 1
					border.color: "#A4A4A4"
					color: "#F7D358"
					anchors.right: parent.right
					anchors.rightMargin: 5
					anchors.verticalCenter: parent.verticalCenter

					Text {
						text: "X"
						font.bold: true
						color: "#585858";
						anchors.centerIn: parent
					}

					MouseArea {
						anchors.fill: parent
						onClicked: {
							__deletedIndex = index
							messagePopup.visible = true
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

				// Focus rect
				Rectangle {
					visible: lazybones.currentMacroPreset.running && (index === lazybones.currentMacroPreset.currentIndex)
					anchors.fill: parent
					border.width: 3
					border.color: "red"
					color: "transparent"
				}
			}
		}
	}
}
