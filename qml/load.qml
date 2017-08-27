import QtQuick 2.0
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
      width : logic.boardSize * squareSize
      height: logic.boardSize * squareSize

      Image {
        source: "/images/chess_board.jpg"
        height: gameBoard.height
        width: gameBoard.width
      }

      Repeater {
        model: logic

        Image {
          height: squareSize
          width : squareSize

          x: squareSize * positionX
          y: squareSize * positionY

          source: images[type].imgPath

//          MouseArea {
//            anchors.fill: parent
//            drag.target: parent

//            property int startX: 0
//            property int startY: 0

//            onPressed: {
//              startX = parent.x;
//              startY = parent.y;
//            }

//            onReleased: {
//              var fromX = startX / squareSize;
//              var fromY = startY / squareSize;
//              var toX   = Math.floor((parent.x + mouseX) / squareSize);
//              var toY   = Math.floor((parent.y + mouseY) / squareSize);

//              if (!logic.move(fromX, fromY, toX, toY)) {
//                parent.x = startX;
//                parent.y = startY;
//              }
//              else{
//                  parent.x = squareSize * positionX;
//                  parent.y = squareSize * positionY;
//              }
//            }
//          }
        }
      }
    }

    Button {
      id: prevButton
      anchors.left: gameBoard.right
      anchors.right: parent.right
      anchors.leftMargin: 10
      anchors.rightMargin: 10

      text: "Previous step"

      onClicked: {
          if (!logic.prevStep()){
              logic.show_msg("A File Error has Occurred! Try another one, please!");
              logic.clear();
          }
      }
    }

    Button {
      id: nextButton
      anchors.left: gameBoard.right
      anchors.right: parent.right
      anchors.top: prevButton.bottom
      anchors.topMargin: 10
      anchors.leftMargin: 10
      anchors.rightMargin: 10

      text: "Next step"

      onClicked: {
          if (!logic.nextStep()){
              logic.show_msg("A File Error has Occurred! Try another one, please!");
              logic.clear();
          }
      }
    }

    Button {
      id: startButton
      anchors.left: gameBoard.right
      anchors.right: parent.right
      anchors.top: nextButton.bottom
      anchors.topMargin: 10
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
      anchors.topMargin: 10
      anchors.leftMargin: 10
      anchors.rightMargin: 10

      text: "Load"

      onClicked: {
          logic.load()
          ld.source = "/qml/load.qml"
      }
    }
}
