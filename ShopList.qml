import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.12
import Qt.labs.qmlmodels 1.0
import com.cyks.CommodityModel 1.0


ColumnLayout {
    RowLayout {
        Layout.leftMargin: 300
        id: searchLayout
        height: 70
        TextField {
            id: searchText
            selectByMouse: true
        }

        Button {
            Layout.leftMargin: 30
            contentItem: Text {
                text: qsTr("筛选")
            }

            background: Rectangle {
                anchors.fill: parent
                radius: 8
            }

            onClicked: filterPopup.open()
        }

        Button {
            Layout.leftMargin: 30
            contentItem: Text {
                text: qsTr("搜索")
            }

            background: Rectangle {
                anchors.fill: parent
                radius: 8
            }

            onClicked: commodityModel.search(searchText.text)
        }

        Popup {
            id: filterPopup
            height: 400
            width: 200

            function changeCondition(index, check) {
                if (check)
                    commodityModel.insertCondition(index)
                else
                    commodityModel.deleteCondition(index)
            }

            // 0 for Food
            // 1 for Cloth
            // 2 for Book
            contentItem: ColumnLayout {
                Text {
                    id: filterLabel
                    text: qsTr("商品类型")
                }

                CheckBox {
                    id: foodSelect
                    text: qsTr("食物")
                    checked: true
                    onCheckedChanged: filterPopup.changeCondition(0, checked)
                }

                CheckBox {
                    id: clothSelect
                    text: qsTr("衣服")
                    checked: true
                    onCheckedChanged: filterPopup.changeCondition(1, checked)
                }

                CheckBox {
                    id: bookSelect
                    text: qsTr("书籍")
                    checked: true
                    onCheckedChanged: filterPopup.changeCondition(2, checked)
                }
            }
        }
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

        Connections {
            target: client;
            function onSignIn(isCustomer) {
                tableView.isLogin = true
                tableView.isLoginCustomer = isCustomer
            }

            function onSignOut() {
                tableView.isLogin = false
            }

            function onBuyCommoditySuccess() {
                commodityModel.search(searchText.text)
            }

            function onChangeCommoditySuccess() {
                commodityModel.search(searchText.text)
            }

            function onDeleteCommoditySuccess() {
                commodityModel.search(searchText.text)
            }
        }

        model: commodityModel

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
                    id: inBuy
                    text: "立即购买"
                    enabled: tableView.isLogin && tableView.isLoginCustomer
                    onTriggered: {
                        inBuyPopup.name = commodityModel.getStringData(viewMenu.currentNum, 0)
                        inBuyPopup.user = commodityModel.getStringData(viewMenu.currentNum, 3)
                        inBuyPopup.price = commodityModel.getDoubleData(viewMenu.currentNum, 4)
                        inBuyPopup.percent = commodityModel.getDoubleData(viewMenu.currentNum, 6)
                        inBuyPopup.stored = commodityModel.getNumberData(viewMenu.currentNum, 5)
                        inBuyPopup.open()
                    }
                }

                Action {
                    text: "加入购物车(未实现)"
                    enabled: tableView.isLogin && tableView.isLoginCustomer
                }

                MenuSeparator {}

                Action {
                    text: "管理商品"
                    enabled: tableView.isLogin && !tableView.isLoginCustomer

                    onTriggered: {
                        manageCommodity.name = commodityModel.getStringData(viewMenu.currentNum, 0)
                        manageCommodity.description = commodityModel.getStringData(viewMenu.currentNum, 1)
                        manageCommodity.user = commodityModel.getStringData(viewMenu.currentNum, 3)
                        manageCommodity.price = commodityModel.getDoubleData(viewMenu.currentNum, 4)
                        manageCommodity.stored = commodityModel.getNumberData(viewMenu.currentNum, 5)
                        manageCommodity.open()
                    }
                }

                Action {
                    text: "下架商品"
                    enabled: tableView.isLogin && !tableView.isLoginCustomer

                    onTriggered: {
                        client.deleteCommodity(commodityModel.getStringData(viewMenu.currentNum, 0),
                                               commodityModel.getStringData(viewMenu.currentNum, 3))
                    }
                }
            }
        }

        Popup {
            id: inBuyPopup
            property int stored: 0
            property double price
            property double percent
            property string name
            property string user

            contentItem: ColumnLayout {
                Text {
                    text: qsTr("输入购买商品的数量")
                }

                RowLayout {
                    Slider {
                        id: storedSlider
                        from: 1
                        to: inBuyPopup.stored
                        enabled: inBuyPopup.stored != 0
                        stepSize: 1
                    }

                    Text {
                        text: storedSlider.value
                    }
                }

                RowLayout {
                    Text {
                        text: qsTr("当前折扣: ")
                    }

                    Text {
                        text: inBuyPopup.percent
                    }

                    Text {
                        text: qsTr("原价: ")
                    }

                    Text {
                        text: inBuyPopup.price
                    }
                }

                RowLayout {
                    Button {
                        contentItem: Text {
                            text: qsTr("确定")
                        }

                        background: Rectangle {
                            radius: 6
                            color: "#21be2b"
                        }

                        onClicked: {
                            onClicked: inBuyPopup.close()
                            client.buyCommodity(inBuyPopup.name, inBuyPopup.user,
                                                inBuyPopup.price * inBuyPopup.percent, storedSlider.value)
                        }
                    }

                    Button {
                        contentItem: Text {
                            text: qsTr("取消")
                        }

                        background: Rectangle {
                            radius: 6
                            color: "#21be2b"
                        }

                        onClicked: inBuyPopup.close()
                    }
                }
            }
        }

        Popup {
            id: manageCommodity
            property int stored: 0
            property double price
            property string name
            property string user
            property string description

            contentItem: ColumnLayout {
                RowLayout {
                    Text {
                        text: qsTr("商品名称: ")
                    }

                    Text {
                        text: manageCommodity.name
                    }
                }

                RowLayout {
                    Text {
                        text: qsTr("商家: ")
                    }

                    Text {
                        text: manageCommodity.user
                    }
                }

                RowLayout {
                    Text {
                        text: qsTr("修改商品描述")
                    }
                    TextField {
                        id: commodityDescription
                        text: manageCommodity.description
                        selectByMouse: true
                        selectionColor: "LightGray"

                        background: Rectangle {
                            radius: 6
                            implicitWidth: 200
                            implicitHeight: 40
                            color: "transparent"
                            border.color: "#21be2b"
                        }
                    }
                }

                RowLayout {
                    Text {
                        text: qsTr("修改价格")
                    }
                    TextField {
                        id: commodityPrice
                        selectByMouse: true
                        selectionColor: "LightGray"
                        text: manageCommodity.price
                        validator: DoubleValidator {
                            bottom: 0
                            top: 65535
                        }

                        background: Rectangle {
                            radius: 6
                            implicitWidth: 200
                            implicitHeight: 40
                            color: "transparent"
                            border.color: "#21be2b"
                        }
                    }
                }

                RowLayout {
                    Text {
                        text: qsTr("修改库存")
                    }
                    TextField {
                        id: commodityStore
                        text: manageCommodity.stored
                        selectByMouse: true
                        selectionColor: "LightGray"
                        validator: IntValidator {
                            bottom: 0
                            top: 65535
                        }

                        background: Rectangle {
                            radius: 6
                            implicitWidth: 200
                            implicitHeight: 40
                            color: "transparent"
                            border.color: "#21be2b"
                        }
                    }
                }

                RowLayout {
                    Button {
                        contentItem: Text {
                            text: qsTr("确定")
                        }

                        background: Rectangle {
                            radius: 6
                            color: "#21be2b"
                        }

                        onClicked: {
                            client.changeCommodity(manageCommodity.name,
                                                   commodityDescription.text,
                                                   manageCommodity.user,
                                                   commodityPrice.text,
                                                   commodityStore.text)
                            manageCommodity.close()
                        }
                    }

                    Button {
                        contentItem: Text {
                            text: qsTr("取消")
                        }

                        background: Rectangle {
                            radius: 6
                            color: "#21be2b"
                        }

                        onClicked: manageCommodity.close()
                    }
                }
            }
        }
    }
}
