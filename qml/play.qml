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
          z: 0

          source: images[type].imgPath
          
          MouseArea {
            anchors.fill: parent
            drag.target: parent

            property int startX: 0
            property int startY: 0

            onPressed: {
              startX = parent.x;
              startY = parent.y;
              parent.z = 100;
            }

            onReleased: {
              parent.z = 0;
              var fromX = startX / squareSize;
              var fromY = startY / squareSize;
              var toX   = Math.floor((parent.x + mouseX) / squareSize);
              var toY   = Math.floor((parent.y + mouseY) / squareSize);

              if (!logic.move(fromX, fromY, toX, toY)) {
                parent.x = startX;
                parent.y = startY;
              }
              else{
                  parent.x = squareSize * positionX;
                  parent.y = squareSize * positionY;
              }
            }
          }
        }
      }
    }

    Button {
      id: clearButton
      anchors.left: gameBoard.right
      anchors.right: parent.right
      anchors.leftMargin: 10
      anchors.rightMargin: 10
      
      text: "Save"

      onClicked: {
        logic.save();
//        ld.source = "/qml/start.qml";
      }
    }
    Button {
      id: stopButton
      anchors.left: gameBoard.right
      anchors.right: parent.right
      anchors.top: clearButton.bottom
      anchors.topMargin: 10
      anchors.leftMargin: 10
      anchors.rightMargin: 10

      text: "Stop"

      onClicked: {
          ld.source = "/qml/start.qml";
      }
    }
}
