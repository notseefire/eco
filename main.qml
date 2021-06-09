import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.12

import com.cyks.Client 1.0

ApplicationWindow {
    id: mainWindow
    title: qsTr(" 电子")
    width: 900
    height: 600
    visible: true
    color: "transparent"
    flags: Qt.FramelessWindowHint | Qt.Window

    background: Rectangle {
        anchors.fill: parent
        color: "#f9f4ec"
        radius: 20
        opacity: 0.9
    }

    Connections {
        target: client;
        function onErrorHappen(err) {
            errContent.text = err
            errWarning.open()
        }

        function onInfoHappen(msg) {
            infoContent.text = msg
            infoPopup.open()
        }
    }





    menuBar: RowLayout {
        width: parent.width
        MouseArea { //为窗口添加鼠标事件
            Layout.fillWidth: true
            Layout.fillHeight: true
            acceptedButtons: Qt.LeftButton //只处理鼠标左键
            property point clickPos: "0,0"
            onPressed: { //接收鼠标按下事件
                clickPos  = Qt.point(mouse.x,mouse.y)
            }
            onPositionChanged: { //鼠标按下后改变位置
                //鼠标偏移量
                var delta = Qt.point(mouse.x-clickPos.x, mouse.y-clickPos.y)

                //如果mainwindow继承自QWidget,用setPos
                mainWindow.setX(mainWindow.x+delta.x)
                mainWindow.setY(mainWindow.y+delta.y)
            }
        }

        RowLayout {
            Layout.alignment: Qt.AlignRight
            Button {
                Layout.alignment: Qt.AlignRight
                text: qsTr("最小化")
                background: Rectangle {
                    anchors.fill: parent
                    radius: 6
                    color: "LightGray"
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: mainWindow.showMinimized()
                }
            }

            Button {
                Layout.alignment: Qt.AlignRight
                text: qsTr("退出")
                background: Rectangle {
                    anchors.fill: parent
                    radius: 6
                    color: "Red"
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        client.saveUserConfig()
                        Qt.quit()
                    }
                }
            }
        }
    }

    header: Rectangle {
        radius: 4
        TabBar {
                id: bar
                width: parent.width
                spacing: 1
                TabButton {
                    text: qsTr("主页")

                    background: Rectangle {
                        anchors.fill: parent
                        radius: 6
                        color: "#adcce4"
                    }
                }
                TabButton {
                    text: qsTr("Discovery")
                    background: Rectangle {
                        anchors.fill: parent
                        radius: 6
                        color: "#adcce4"
                    }
                }
                TabButton {
                    text: qsTr("用户中心")
                    background: Rectangle {
                        anchors.fill: parent
                        radius: 6
                        color: "#adcce4"
                    }
                }
        }
    }

    StackLayout {
        anchors.top: header.bottom
        anchors.topMargin: 100
        anchors.leftMargin: 100
        currentIndex: bar.currentIndex
        id: mainContent


        Loader {
            id: home
            source: "qrc:/Home.qml"
        }

        Loader {
            id: shopList
            source: "qrc:/ShopList.qml"
        }


        Loader {
            id: userHub
            source: "qrc:/UserHub.qml"
        }

        Popup {
            id: errWarning
            ColumnLayout {
                Text {
                    text: qsTr("错误")
                }

                Text {
                    id: errContent
                }

                Button {
                    contentItem: Text {
                        text: qsTr("确定")
                    }

                    background: Rectangle {
                        radius: 6
                        color: "#21be2b"
                    }

                    onClicked: {
                        onClicked: errWarning.close()
                    }
                }
            }
        }

        Popup {
            id: infoPopup
            ColumnLayout {
                Text {
                    text: qsTr("提示")
                }

                Text {
                    id: infoContent
                }

                Button {
                    contentItem: Text {
                        text: qsTr("确定")
                    }

                    background: Rectangle {
                        radius: 6
                        color: "#21be2b"
                    }

                    onClicked: {
                        onClicked: infoPopup.close()
                    }
                }
            }
        }
    }



}
