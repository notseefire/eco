import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtQuick.Window 2.12

Component {
    ColumnLayout {
        Text {
            text: qsTr("购物车")
        }

        TableView {
            property bool isLogin: false
            property bool isLoginCustomer: false
            Layout.topMargin: 100
            Layout.leftMargin: 200
            implicitWidth: 600
            implicitHeight: 300
            id: tableView

            columnWidthProvider: function(column) {
                return 100;
            }

            rowHeightProvider: function(row) {
                return 50;
            }

            model: cartModel

            ScrollBar.vertical:   ScrollBar {
                anchors.right: tableView.right
                width: 5
                active: true

                contentItem: Rectangle {
                    id:contentItem_rect2
                    width:10
                    height:30
                    radius: implicitHeight/2
                    color: "LightGray"
                }
            }

            Row {
                id: columnsHeader
                y: tableView.contentY - 50
                z: 2
                Repeater {
                    model: tableView.columns > 0 ? tableView.columns : 1
                    Label {
                        width: tableView.columnWidthProvider(modelData)
                        height: 35
                        text: commodityModel.headerData(modelData, Qt.Horizontal)
                        color: '#aaaaaa'
                        font.pixelSize: 15
                        padding: 10
                        verticalAlignment: Text.AlignVCenter

                        background: Rectangle { color: "#333333" }
                    }
                }
            }

            delegate: Rectangle {
                width:100
                height: 50

                Text {
                    padding: 5
                    anchors.fill: parent
                    text: display
                    wrapMode: Text.WordWrap
                    anchors.centerIn: parent
                }

                MouseArea {
                    anchors.fill: parent
                    acceptedButtons: Qt.RightButton
                    onClicked: {
                        viewMenu.currentNum = row
                        viewMenu.popup()
                    }
                }

                Menu {
                    id: viewMenu
                    property int currentNum: 0

                    Action {
                        text: "移除商品"
                    }
                }
            }
        }
        Button {
            text: qsTr("购物车结算")
        }
    }
}


