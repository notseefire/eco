import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12


Item {
    ColumnLayout {
        anchors.left: parent.left
        anchors.leftMargin: 200
        Frame {
            contentItem: Text {
                text: qsTr("ECO电商交易平台1.0ver")
            }
        }

        Frame {
            contentItem: Text {
                text: qsTr("作者: CYKS")
            }
        }
    }
}
