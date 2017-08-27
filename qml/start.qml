import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2

Item {
    width: 800
    height: 600
    Item {
      id: gameBoard
      x: 0
      y: 0
      width : logic.boardSize * root.squareSize
      height: logic.boardSize * root.squareSize

      Image {
        source: "/images/chess_board.jpg"
        height: gameBoard.height
        width: gameBoard.width
      }
    }

    Button {
      id: startButton
      anchors.left: gameBoard.right
      anchors.right: parent.right
      anchors.leftMargin: 10
      anchors.rightMargin: 10

      text: "Start"

      onClicked: {
         logic.start()
         ld.source = "/qml/play.qml"
      }
    }

    Button {
      id: loadButton
      anchors.left: gameBoard.right
      anchors.right: parent.right
      anchors.top: startButton.bottom
      anchors.leftMargin: 10
      anchors.topMargin: 10
      anchors.rightMargin: 10

      text: "Load"

      onClicked: {
          logic.load()
          ld.source = "/qml/load.qml"
      }
    }
}
