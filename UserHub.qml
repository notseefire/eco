import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12


Item {
    property bool islogin: false
    property bool isLoginCustomer: false
    property double balance
    property string userid

    Connections {
        target: client;
        function onSignIn(isCustomer) {
            islogin = true
            isLoginCustomer = isCustomer
            userid = client.getUserId()
        }

        function onSignOut() {
            islogin = false
        }

        function onBalanceChange(n_balance) {
            balance = n_balance
        }
    }

    StackLayout {
        anchors.horizontalCenter: parent.horizontalCenter
        currentIndex: islogin ? 1 : 0

        ColumnLayout {
            Switch {
                id: rlswitch
                text: rlswitch.checked ? qsTr("注册") : qsTr("登录")
            }

            StackLayout {
                currentIndex: rlswitch.checked ? 0 : 1;

                Loader {
                    id: register
                    source: "qrc:/Register.qml"
                }

                Loader {
                    id: login
                    source: "qrc:/Login.qml"
                }
            }
        }

        ColumnLayout {

            Rectangle {
                id: userbaseUI
                ColumnLayout {

                    Frame{
                        contentItem: RowLayout {
                            Text {
                                text: qsTr("用户名称: ")
                            }

                            Text {
                                text: userid
                            }
                        }
                    }


                    Frame{
                        contentItem: RowLayout {
                            Text {
                                text: qsTr("用户类型: ")
                            }

                            Text {
                                text: isLoginCustomer ? "客户" : "商家"
                            }
                        }
                    }

                    Button {
                        contentItem: Text {
                            text: qsTr("更改密码")
                        }

                        background: Rectangle {
                            radius: 6
                        }

                        onClicked: changePasswordPopup.open()
                    }

                    Button {
                        contentItem: Text {
                            text: qsTr("登出")
                        }

                        background: Rectangle {
                            radius: 6
                        }

                        onClicked: client.signOutUser()
                    }
                }
            }

            StackLayout {
                Layout.topMargin: 200
                currentIndex: isLoginCustomer ? 0 : 1

                Item {
                    id: customer

                    ColumnLayout {
                        RowLayout {
                            Text {
                                text: qsTr("用户余额")
                            }

                            Text {
                                text: balance.toFixed(2)
                            }
                        }

                        Button {
                            id: rechargeMoney
                            contentItem: Text {
                                text: qsTr("充值")
                            }
                            onClicked: rechargePopup.open()
                        }
                    }

                    Popup {
                        id: rechargePopup

                        contentItem: ColumnLayout {
                            Text {
                                text: qsTr("输入充值的金额")
                            }

                            RowLayout {
                                Slider {
                                    id: chargeSlider
                                    from: 0
                                    to: 1000
                                    stepSize: 1
                                }

                                Text {
                                    text: chargeSlider.value
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
                                        onClicked: rechargePopup.close()
                                        client.recharge(chargeSlider.value)
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

                                    onClicked: rechargePopup.close()
                                }
                            }
                        }
                    }
                }

                ColumnLayout {
                    id: seller

                    Button {
                        anchors.topMargin: 200
                        contentItem: Text {
                            id: addCommodity
                            text: qsTr("添加商品")

                        }
                        onClicked: addCommodityPopup.open()
                    }

                    Button {
                        Layout.topMargin: 50
                        contentItem: Text {
                            id: addEvent
                            text: qsTr("打折活动")
                        }
                        onClicked: addEventPopup.open()
                    }

                    Popup {
                        id: addCommodityPopup
                        anchors.centerIn: Overlay.overlay
                        height: 600
                        width: 400

                        contentItem: ColumnLayout {
                            id: popupContent

                            function clearTextField() {
                                commodityName.clear()
                                commodityPrice.clear()
                                commodityDescription.clear()
                                commodityStore.clear()
                            }

                            Text {
                                text: qsTr("添加商品")
                            }

                            RowLayout {
                                Text {
                                    text: qsTr("商品名称")
                                }
                                TextField {
                                    id: commodityName
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
                                    text: qsTr("商品描述")
                                }
                                TextField {
                                    id: commodityDescription
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
                                    text: qsTr("商品类型")
                                }
                                ComboBox {
                                    id: commodityType
                                    model: ["Book", "Cloth", "Food"]
                                }
                            }

                            RowLayout {
                                Text {
                                    text: qsTr("价格")
                                }
                                TextField {
                                    id: commodityPrice
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
                                    text: qsTr("库存")
                                }
                                TextField {
                                    id: commodityStore
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
                                Button {
                                    contentItem: Text {
                                        text: qsTr("确定")
                                    }

                                    background: Rectangle {
                                        implicitWidth: 100
                                        implicitHeight: 40
                                        color: "#21be2b"
                                    }

                                    onClicked: {
                                        client.addNewCommoidty(commodityName.text, commodityDescription.text, commodityType.currentText,
                                                               commodityPrice.text, commodityStore.text)
                                        addCommodityPopup.close()
                                        popupContent.clearTextField()
                                    }
                                }

                                Button {
                                    contentItem: Text {
                                        text: qsTr("取消")
                                    }

                                    background: Rectangle {
                                        implicitWidth: 100
                                        implicitHeight: 40
                                        color: "#21be2b"
                                    }

                                    onClicked: {
                                        addCommodityPopup.close()
                                        popupContent.clearTextField()
                                    }
                                }
                            }
                        }
                    }

                    Popup {
                        id: addEventPopup

                        contentItem: ColumnLayout {
                                RowLayout {
                                    Text {
                                        text: qsTr("活动商品类型")
                                    }
                                    ComboBox {
                                        id: eventType
                                        model: ["Book", "Cloth", "Food"]
                                    }
                                }

                                RowLayout {
                                    Slider {
                                        id: percentSlider
                                        from: 0.1
                                        to: 1
                                        stepSize: 0.1
                                    }

                                    Text {
                                        text: (percentSlider.value.toFixed(1) * 10) + qsTr("折")
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
                                        client.addEvent(eventType.currentText, percentSlider.value)
                                        addEventPopup.close()
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

                                    onClicked: addEventPopup.close()
                                }
                            }
                        }
                    }

                    Popup {
                        id: changePasswordPopup

                        contentItem: ColumnLayout {
                            RowLayout {
                                Text {
                                    text: qsTr("原密码")
                                }

                                TextField {
                                    id: oldPassword
                                    selectByMouse: true
                                    selectionColor: "LightGray"
                                    echoMode: TextInput.Password
                                }
                            }

                            RowLayout {
                                Text {
                                    text: qsTr("新密码")
                                }

                                TextField {
                                    id: newPassword
                                    selectByMouse: true
                                    selectionColor: "LightGray"
                                    echoMode: TextInput.Password
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
                                        client.changePassword(oldPassword.text, newPassword.text);
                                        changePasswordPopup.close()
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

                                    onClicked: changePasswordPopup.close()
                                }
                            }


                        }
                    }
                }
            }
        }


    }
}
