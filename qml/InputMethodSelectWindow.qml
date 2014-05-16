import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Window 2.1
import QtQuick.Dialogs 1.1
import gunoodaddy 1.0

Window {
	id: inputWindow
	property bool __addMode: false

	property InputAction inputAction: null

	width: 465
	height: 370
	modality: Qt.WindowModal

	signal actionAdded();
	signal actionUpdated();

	property list<QtObject> keyboardModel: [
		QtObject { property string name: "1"; property int keyCode: Qt.Key_1; property Button buttonObj: null; property int buttonWidth: 25; },
		QtObject { property string name: "2"; property int keyCode: Qt.Key_2; property Button buttonObj: null; property int buttonWidth: 25; },
		QtObject { property string name: "3"; property int keyCode: Qt.Key_3; property Button buttonObj: null; property int buttonWidth: 25; },
		QtObject { property string name: "4"; property int keyCode: Qt.Key_4; property Button buttonObj: null; property int buttonWidth: 25; },
		QtObject { property string name: "Q"; property int keyCode: Qt.Key_Q; property Button buttonObj: null; property int buttonWidth: 25; },
		QtObject { property string name: "T"; property int keyCode: Qt.Key_T; property Button buttonObj: null; property int buttonWidth: 25; },
		QtObject { property string name: "Esc"; property int keyCode: Qt.Key_Escape; property Button buttonObj: null; property int buttonWidth: 40; },
		QtObject { property string name: "Space"; property int keyCode: Qt.Key_Space; property Button buttonObj: null; property int buttonWidth: 60; },
		QtObject { property string name: "C"; property int keyCode: Qt.Key_C; property Button buttonObj: null; property int buttonWidth: 25; }
	]

	MessageDialog {
		id: messagePopup
	}

	property CaptureWindow captureWindow: CaptureWindow {
		visible: false

		onVisibleChanged: {
			if(!visible) {
				inputWindow.raise()
				inputWindow.requestActivate()
			}
		}

		onClicked: {
			inputAction.mouseX = captureWindow.mouseX
			inputAction.mouseY = captureWindow.mouseY
			updateData()
		}
	}

	function updateActionText() {
		var str = inputAction.description;
		actionText.text = str
	}

	function updateData() {
		if(inputAction.actionType === InputAction.KeyPress) {
			for(var i = 0; i < keyboardModel.length; i++) {
				if(keyboardModel[i].buttonObj) {
					keyboardModel[i].buttonObj.checked = false
					if(keyboardModel[i].keyCode === inputAction.keyCode) {
						keyboardModel[i].buttonObj.checked = true
					}
				}
			}
		}

		xPosField.text = inputAction.mouseX
		yPosField.text = inputAction.mouseY
		delayedField.text = inputAction.delayedMsec

		updateActionText();
	}

	onVisibleChanged: {
		if(visible) {
			if(inputAction === null) {
				__addMode = true
				inputAction = lazybones.createInputAction()
			} else {
				__addMode = false
			}

			updateData()
		}
	}

	ExclusiveGroup {
		id: radioGroup
	}

	Rectangle {
		anchors.fill: parent
		anchors.margins: 5

		GroupBox {
			id: keybordButtonBox
			anchors.left: parent.left
			width: 160
			title: "Keyboard"
			Flow {
				anchors.fill: parent
				spacing: 4

				Repeater {
					model: keyboardModel
					Button {
						width: buttonWidth
						checkable: true
						exclusiveGroup: radioGroup
						text: name

						onClicked: {
							inputAction.actionType = InputAction.KeyPress
							inputAction.keyCode = keyCode
							updateActionText()
						}
						Component.onCompleted: {
							buttonObj = this
						}
					}
				}
			}
		}

		GroupBox {
			id: inputActionBox
			anchors.left: keybordButtonBox.right
			anchors.leftMargin: 5
			title: "Mouse"
			Column {
				spacing: 4
				Row {
					Text {
						anchors.verticalCenter: parent.verticalCenter
						text: "Left "
						width: 45
					}

					Button {
						id: buttonLeftClick
						text: "Click"
						checkable: true
						checked: inputAction !== null && inputAction.actionType === InputAction.MouseLeftClick
						exclusiveGroup: radioGroup
						onClicked: {
							inputAction.actionType = InputAction.MouseLeftClick
							updateActionText()
						}
					}
					Button {
						id: buttonLeftPress
						text: "Press"
						checkable: true
						checked: inputAction !== null && inputAction.actionType === InputAction.MouseLeftPress
						exclusiveGroup: radioGroup
						onClicked: {
							inputAction.actionType = InputAction.MouseLeftPress
							updateActionText()
						}
					}
					Button {
						id: buttonLeftRelease
						text: "Release"
						checkable: true
						checked: inputAction !== null && inputAction.actionType === InputAction.MouseLeftRelease
						exclusiveGroup: radioGroup
						onClicked: {
							inputAction.actionType = InputAction.MouseLeftRelease
							updateActionText()
						}
					}
				}

				Row {
					Text {
						anchors.verticalCenter: parent.verticalCenter
						text: "Right "
						width: 45
					}

					Button {
						id: buttonRightClick
						text: "Click"
						checkable: true
						checked: inputAction !== null && inputAction.actionType === InputAction.MouseRightClick
						exclusiveGroup: radioGroup
						onClicked: {
							inputAction.actionType = InputAction.MouseRightClick
							updateActionText()
						}
					}
					Button {
						id: buttonRightPress
						text: "Press"
						checkable: true
						checked: inputAction !== null && inputAction.actionType === InputAction.MouseRightPress
						exclusiveGroup: radioGroup
						onClicked: {
							inputAction.actionType = InputAction.MouseRightPress
							updateActionText()
						}
					}
					Button {
						id: buttonRightRelease
						text: "Release"
						checkable: true
						checked: inputAction !== null && inputAction.actionType === InputAction.MouseRightRelease
						exclusiveGroup: radioGroup
						onClicked: {
							inputAction.actionType = InputAction.MouseRightRelease
							updateActionText()
						}
					}
				}

				Button {
					id: buttonMove
					width: parent.width
					text: "Cursor Move"
					checkable: true
					checked: inputAction !== null && inputAction.actionType === InputAction.MouseMove
					exclusiveGroup: radioGroup
					onClicked: {
						inputAction.actionType = InputAction.MouseMove
						updateActionText()
					}
				}

				Row {
					spacing: 5

					Column {
						spacing: 5

						TextField {
							width: 60
							id: xPosField
							placeholderText: "X pos"
							inputMethodHints: Qt.ImhDigitsOnly

							onTextChanged: {
								inputAction.mouseX = text
								updateActionText()
							}
						}

						TextField {
							width: 60
							id: yPosField
							placeholderText: "Y pos"
							inputMethodHints: Qt.ImhDigitsOnly

							onTextChanged: {
								inputAction.mouseY = text
								updateActionText()
							}
						}
					}

					Button {
						text: "Click"
						height: parent.implicitHeight
						onClicked: {
							captureWindow.show()
						}
					}

					Button {
						text: "Clear"
						height: parent.implicitHeight
						onClicked: {
							yPosField.text = -1
							xPosField.text = -1
							inputAction.mouseX = -1
							inputAction.mouseY = -1
							updateActionText()
						}
					}
				}
			}
		}

		Column {
			anchors.bottom: parent.bottom
			anchors.bottomMargin: 50
			anchors.left: parent.left
			anchors.right: parent.right
			spacing: 10

			Row {
				height: 20
				spacing: 3
				Text {
					text: "Run action after "
					anchors.verticalCenter: parent.verticalCenter
				}

				TextField {
					id: delayedField
					placeholderText: "Delay millisecond"
					inputMethodHints: Qt.ImhDigitsOnly

					onTextChanged: {
						if(text.length > 0)
						{
							inputAction.delayedMsec = text
						}
						else
						{
							inputAction.delayedMsec = 0
						}
						updateActionText()
					}
				}

				Text {
					text: "msec."
					anchors.verticalCenter: parent.verticalCenter
				}

			}

			GroupBox {
				title: "Action"
				anchors.left: parent.left
				anchors.right: parent.right

				Text {
					id: actionText
				}
			}

			Button {
				text: "Test"
				width: parent.width
				onClicked: {
					inputAction.test()
				}
			}
		}
	}

	Row {
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.bottom: parent.bottom
		spacing: 10
		anchors.bottomMargin: 10
		Button {
			id: okButton
			visible: __addMode
			text: "OK"
			onClicked: {

				if(inputAction.actionType <= 0) {
					messagePopup.text = "Select a action type."
					messagePopup.visible = true
					return
				}

				if(__addMode)
					actionAdded()
				else
					actionUpdated()

				hide()
			}
		}

		Button {
			id: cancelButton
			text: __addMode ? "Cancel" : "Close"
			onClicked: {
				inputWindow.hide()
			}
		}
	}
}
