import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

import com.cyks.Client 1.0

Item {
    ColumnLayout {
        anchors.horizontalCenter: parent.horizontalCenter

        RowLayout {
            Layout.topMargin: 10
            Text {
                id: useridLabel
                text: qsTr("用户名:\t")
            }

            TextField {
                id: userid
                selectByMouse: true
                selectionColor: "LightGray"
                placeholderText: qsTr("请输入用户名")
            }
        }


        RowLayout {
            Text {
                id: passwordLabel
                text: qsTr("密码:\t")
            }

            TextField {
                id: password
                selectByMouse: true
                selectionColor: "LightGray"
                placeholderText: qsTr("请输入密码")
                echoMode: TextInput.Password
            }
        }


        Button {
            contentItem: Text {
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter
                id: confirmButton
                text: qsTr("确定")
            }

            background: Rectangle {
                anchors.fill: parent
                radius: 6
                color: "#e4aebb"
            }

            onClicked: {
                client.loginUser(userid.text, password.text)
                password.clear()
            }
        }
    }
}
