import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
import org.github.henryquan.bridge 1.0

ApplicationWindow {
    id: rootWindow
    visible: true
    width: 640
    height: 480
    title: qsTr("パズそば - Puzzle & Dragons Solver")

    // Make sure you actually use it or it never works
    Bridge {
        id: soba
    }

    onVisibleChanged: {
        console.log(soba.initialBoard)
    }

    Grid {
        id: grid
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 16
        anchors.horizontalCenter: parent.horizontalCenter
        columns: soba.column
        rows: soba.row
        spacing: 4
        Repeater {
            model: soba.initialBoard.length
            Image {
                id: orbImage
                source: {
                    return `images/${soba.initialBoard[index]}.png`;
                }
            }
        }
    }
}
