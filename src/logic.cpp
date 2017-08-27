#include "logic.h"
#include <QList>
#include <QByteArray>
#include <QHash>
#include <QtMath>
#include <iostream>
#include <QFileDialog>
#include <QTextStream>
#include <QDebug>
#include <QMessageBox>
#include <QRegExp>
#include <QRegularExpressionMatch>

typedef bool (Logic::*func_pointer)(int fromX, int fromY, int toX, int toY, int type);

struct Figure
{
  int type;
  int x;
  int y;
};


struct Logic::Impl
{
  QList<Figure> figures;

  int findByPosition(int x, int y);
};

int Logic::Impl::findByPosition(int x, int y) {
  for (int i(0); i<figures.size(); ++i) {
    if (figures[i].x != x || figures[i].y != y ) { 
      continue; 
    }
    return i;    
  }
  return -1;
}


Logic::Logic(QObject *parent)
  : QAbstractListModel(parent)
  , impl(new Impl())
{
    return ;
//    this->start();
}

Logic::~Logic() {
}

int Logic::boardSize() const {
  return BOARD_SIZE;
}

int Logic::rowCount(const QModelIndex & ) const {
  return impl->figures.size();
}

QHash<int, QByteArray> Logic::roleNames() const { 
  QHash<int, QByteArray> names;
  names.insert(Roles::Type      , "type");
  names.insert(Roles::PositionX , "positionX");
  names.insert(Roles::PositionY , "positionY");
  return names;
}

QVariant Logic::data(const QModelIndex & modelIndex, int role) const { 
  if (!modelIndex.isValid()) {
    return QVariant();
  }
    
  int index = static_cast<int>(modelIndex.row());
  
  if (index >= impl->figures.size() || index < 0) {
    return QVariant();
  }

  Figure & figure = impl->figures[index];
    
  switch (role) {
    case Roles::Type     : return figure.type;
    case Roles::PositionX: return figure.x;
    case Roles::PositionY: return figure.y;
  }
  return QVariant(); 
}

bool Logic::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid()) {
        return false;
    }

    switch (role) {
    case Roles::Type:
        return false;    // This property can not be set
    case Roles::PositionX:
       impl->figures[index.row()].x = value.toInt();
        break;
    case Roles::PositionY:
       impl->figures[index.row()].y = value.toInt();
        break;
    default:
        return false;
    }

    emit dataChanged(index, index, QVector<int>() << role);

    return true;
}

bool Logic::removeRows(int index, int rows, const QModelIndex &parent) {
     (void)parent;
     beginRemoveRows(QModelIndex(), index, index + rows - 1);

     for (int row = 0; row < rows; ++row) {
         impl->figures.removeAt(index);
     }

     endRemoveRows();
     return true;
}

void Logic::remove_by_pos(int x, int y){
     int index = impl->findByPosition(x, y);

     this->removeRows(index, 1, QModelIndex());
}

void Logic::start() {
    this->clear();
    beginInsertRows(QModelIndex(), 0, 31);
    for (int i = 0; i < this->boardSize(); ++i) {
          impl->figures << Figure { WHITE_PAWN, i, 1 };
          impl->figures << Figure { BLACK_PAWN, i, 6 };
    }

    impl->figures << Figure { WHITE_ROOK, 0, 0 };
    impl->figures << Figure { WHITE_ROOK, 7, 0 };
    impl->figures << Figure { WHITE_KNIGHT, 1, 0 };
    impl->figures << Figure { WHITE_KNIGHT, 6, 0 };
    impl->figures << Figure { WHITE_BISHOP, 2, 0 };
    impl->figures << Figure { WHITE_BISHOP, 5, 0 };
    impl->figures << Figure { WHITE_QUEEN, 4, 0 };
    impl->figures << Figure { WHITE_KING, 3, 0 };

    impl->figures << Figure { BLACK_ROOK, 0, 7 };
    impl->figures << Figure { BLACK_ROOK, 7, 7 };
    impl->figures << Figure { BLACK_KNIGHT, 1, 7 };
    impl->figures << Figure { BLACK_KNIGHT, 6, 7 };
    impl->figures << Figure { BLACK_BISHOP, 2, 7 };
    impl->figures << Figure { BLACK_BISHOP, 5, 7 };
    impl->figures << Figure { BLACK_QUEEN, 4, 7 };
    impl->figures << Figure { BLACK_KING, 3, 7 };
    endInsertRows();
    this->firstMove = true;
}

void Logic::load(QString fileName) {
    QRegExp rx1("^(Move)[|]{1}[0-7]{1}[,]{1}[0-7]{1}[,]{1}[0-7]{1}[,]{1}[0-7]{1}$");
    QRegExp rx2("^(Delete)[|]{1}[0-9]{1,2}[,]{1}[0-7]{1}[,]{1}[0-7]{1}[;]{1}(Move)[|]{1}[0-7]{1}[,]{1}[0-7]{1}[,]{1}[0-7]{1}[,]{1}[0-7]{1}$");
    fileName = QSysInfo::productType() == "windows" ? fileName.right(fileName.length() - 8):
                                                      fileName.right(fileName.length() - 7);
    QFile file(fileName);

    this->start();
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
          show_msg("error was detected while loading file!");
          return;
    }
    step = 0;
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        if (rx1.indexIn(line) == -1 && rx2.indexIn(line) == -1){
            show_msg("error was detected while loading file!");
            clear();
            return;
        }
        this->loadedFile << line;
    }
}

void Logic::save(QString fileName) {
    fileName = fileName.right(3) == ".sf" ? fileName : fileName + ".sf";
    fileName = QSysInfo::productType() == "windows" ? fileName.right(fileName.length() - 8):
                                                      fileName.right(fileName.length() - 7);
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)){
          show_msg("error was detected while saving file!");
          return;
    }

    QTextStream out(&file);
    QStringList::Iterator it = saveList.begin();

    while (it != saveList.end()) {
        out << *it;
        it++;
    }
}

void Logic::clear() {
  beginResetModel();
  impl->figures.clear();
  this->saveList.clear();
  this->loadedFile.clear();
  endResetModel();
}

void Logic::show_msg(const QString &msg){
    QMessageBox msgBox;
    msgBox.setText(msg);
    msgBox.exec();
}

bool Logic::prevStep(){
    if (loadedFile.isEmpty() || step == 0)
        return true;
    step--;

    QVector <Action> act = parse_step(step);
    if (act.isEmpty())
        return false;
    func_pointer move_func[] = {act.length() == 2 ?
                                &Logic::pawns_move_beat : &Logic::pawns_move,
                                &Logic::knight_move, &Logic::rook_move, &Logic::bishop_move,
                                &Logic::queen_move, &Logic::king_move};
    if (act.length() == 2) {
            int index = impl->findByPosition(act[1].args[2], act[1].args[3]);
            if (index < 0) return false;

            int type = impl->figures[index].type;
            if ((this->*move_func[(type) % 6])(act[1].args[0], act[1].args[1], act[1].args[2], act[1].args[3], type)){

                this->setData(createIndex(index, 0), QVariant(act[1].args[0]), Roles::PositionX);
                this->setData(createIndex(index, 0), QVariant(act[1].args[1]), Roles::PositionY);
                beginInsertRows(QModelIndex(), impl->figures.length(), impl->figures.length());
                impl->figures << Figure {act[0].args[0], act[0].args[1], act[0].args[2]};
                endInsertRows();

            } else
                return false;
    } else if (act.length() == 1){
            int index = impl->findByPosition(act[0].args[2], act[0].args[3]);
            if (index < 0) return false;

            int type = impl->figures[index].type;
            if ((this->*move_func[(type) % 6])(act[0].args[0], act[0].args[1], act[0].args[2], act[0].args[3], type)){
                this->setData(createIndex(index, 0), QVariant(act[0].args[0]), Roles::PositionX);
                this->setData(createIndex(index, 0), QVariant(act[0].args[1]), Roles::PositionY);
            } else
                return false;
    }
    return true;
}

bool Logic::nextStep(){
    if (loadedFile.isEmpty() || step == loadedFile.length())
        return true;
    step++;
    QVector <Action> act = parse_step(step - 1);
    if (act.isEmpty())
        return false;
    func_pointer move_func[] = {act.length() == 2 ?
                                &Logic::pawns_move_beat : &Logic::pawns_move,
                                &Logic::knight_move, &Logic::rook_move, &Logic::bishop_move,
                                &Logic::queen_move, &Logic::king_move};
    if (act.length() == 2) {
            int index = impl->findByPosition(act[1].args[0], act[1].args[1]);
            if (index < 0) return false;

            int type = impl->figures[index].type;
            if ((this->*move_func[(type) % 6])(act[1].args[0], act[1].args[1], act[1].args[2], act[1].args[3], type)){
                index = impl->findByPosition(act[0].args[1], act[0].args[2]);
                if (index < 0) return false;
                this->remove_by_pos(act[0].args[1], act[0].args[2]);
                index = impl->findByPosition(act[1].args[0], act[1].args[1]);
                this->setData(createIndex(index, 0), QVariant(act[1].args[2]), Roles::PositionX);
                this->setData(createIndex(index, 0), QVariant(act[1].args[3]), Roles::PositionY);
            } else
                return false;
    } else if (act.length() == 1){
            int index = impl->findByPosition(act[0].args[0], act[0].args[1]);
            if (index < 0) return false;

            int type = impl->figures[index].type;
            if ((this->*move_func[(type) % 6])(act[0].args[0], act[0].args[1], act[0].args[2], act[0].args[3], type)){
                this->setData(createIndex(index, 0), QVariant(act[0].args[2]), Roles::PositionX);
                this->setData(createIndex(index, 0), QVariant(act[0].args[3]), Roles::PositionY);
            } else
                return false;
    }
    return true;
}

QVector <Action> Logic::parse_step(int step){
     QVector <Action> act;

     QString str = loadedFile.at(step);
     QStringList commands = str.split(";");
     foreach (const QString &com, commands) {
         QStringList parts =  com.split("|");
         QString name = parts.at(0);

         QStringList argList = parts.at(1).split(",");
         QList <int> args;
         foreach (const QString &arg, argList)
             args << arg.toInt();
         act << Action { name, args };
     }
         if (act.length() == 2 && (act[0].args[0] > 11 ||
             (act[1].args[2] != act[0].args[1] && act[1].args[3] != act[0].args[2])))
             act.clear();
     return act;
}

bool Logic::pawns_move(int fromX, int fromY, int toX, int toY, int type){

     int step = 1;
     if ((type == WHITE_PAWN && fromY == 1) || (type == BLACK_PAWN && fromY == 6))
             step++;
     if (fromX != toX)
         return false;
     if (type == WHITE_PAWN && (toY - fromY > step || toY - fromY < 0))
         return false;
     else if (type == BLACK_PAWN && (fromY - toY > step || fromY - toY < 0))
         return false;
     if (step == 2 && !(toY - fromY == 1 || toY - fromY == -1)){
         if (type == WHITE_PAWN && impl->findByPosition(toX, toY - 1) >= 0)
             return false;
         if (type == BLACK_PAWN && impl->findByPosition(toX, toY + 1) >= 0)
             return false;
     }
     return true;
}

bool Logic::pawns_move_beat(int fromX, int fromY, int toX, int toY, int type){

    if (type == WHITE_PAWN && (toY - fromY != 1 || qFabs(toX - fromX) != 1))
        return false;
    if (type == BLACK_PAWN && (-(toY - fromY) != 1 || qFabs(toX - fromX) != 1))
        return false;
    return true;
}

bool Logic:: bishop_move(int fromX, int fromY, int toX, int toY, int type){
        (void)type;
        if (qFabs(fromX - toX) != qFabs(fromY - toY))
            return false;
        for (int i = 1; i < qFabs(fromX - toX); ++i) {
            if (toX > fromX && toY > fromY){
                if (impl->findByPosition(fromX + i, fromY + i) >= 0)
                    return false;
                else
                    continue;
            }
            if (toX < fromX && toY < fromY){
                if (impl->findByPosition(fromX - i, fromY - i) >= 0)
                    return false;
                else
                    continue;
            }
            if (toX < fromX && toY > fromY){
                if (impl->findByPosition(fromX - i, fromY + i) >= 0)
                    return false;
                else
                    continue;
            }
            if (toX > fromX && toY < fromY){
                if (impl->findByPosition(fromX + i, fromY - i) >= 0)
                    return false;
                else
                    continue;
            }
        }
    return true;
}

bool Logic::rook_move(int fromX, int fromY, int toX, int toY, int type){
    (void)type;
    int direction = fromY == toY ? 1 : (fromX == toX ? 2 : 0); //1 - hor; 2 - ver; 0 - error
    if (!direction)
        return false;
    int fromVal = direction == 1 ? fromX : fromY;
    int toVal = direction == 1 ? toX : toY;
    for (int i = 1; i < qFabs(fromVal - toVal); ++i) {
        if (direction == 1){
            if (fromVal - toVal < 0 && impl->findByPosition(fromX + i, toY) >= 0)
                return false;
            if (fromVal - toVal > 0 && impl->findByPosition(fromX - i, toY) >= 0)
                return false;
        }
        else {
            if (fromVal - toVal < 0 && impl->findByPosition(toX, fromY + i) >= 0)
                return false;
            if (fromVal - toVal > 0 && impl->findByPosition(toX, fromY - i) >= 0)
                return false;
        }
    }
    return true;
}

bool Logic::knight_move(int fromX, int fromY, int toX, int toY, int type){
    (void)type;
    if (!((qFabs(fromX - toX) == 2 && qFabs(fromY - toY) == 1)
          || (qFabs(fromX - toX) == 1 && qFabs(fromY - toY) == 2)))
        return false;
    return true;
}

bool Logic::king_move(int fromX, int fromY, int toX, int toY, int type){
    (void)type;
    if (qFabs(fromX - toX) > 1 || qFabs(fromY - toY) > 1)
        return false;
    return true;
}

bool Logic::queen_move(int fromX, int fromY, int toX, int toY, int type){
    bool exit;
    exit = this->rook_move(fromX, fromY, toX, toY, type);
    if (!exit)
        exit = this->bishop_move(fromX, fromY, toX, toY, type);
    if (!exit)
        return false;

    return true;
}

bool Logic::move(int fromX, int fromY, int toX, int toY) {
  static char queue = 1;                                            //1 - white; 2 - black
  int index = impl->findByPosition(fromX, fromY);
  int type;
  QString log;
  func_pointer move_func[] = {impl->findByPosition(toX, toY) >= 0 ?
                              &Logic::pawns_move_beat : &Logic::pawns_move,
                              &Logic::knight_move, &Logic::rook_move, &Logic::bishop_move,
                              &Logic::queen_move, &Logic::king_move};



  if (index < 0) return false;

  if (this->firstMove){
      queue = 1;
      this->firstMove = false;
  }

  type = impl->figures[index].type;

  if ((queue == 2 && (WHITE_PAWN <= type && type <= WHITE_KING)) ||
         (queue == 1 && (BLACK_PAWN <= type && type <= BLACK_KING)))
      return false;

  if (toX < 0 || toX >= BOARD_SIZE || toY < 0 || toY >= BOARD_SIZE)
    return false;

  if (!(this->*move_func[(type) % 6])(fromX, fromY, toX, toY, type))
      return false;

  if (impl->findByPosition(toX, toY) >= 0){
      int removeIndex = impl->findByPosition(toX, toY);
      int removeType = impl->figures[removeIndex].type;
      if ((queue == 2 && (BLACK_PAWN <= removeType && removeType <= BLACK_KING)) ||
          (queue == 1 && (WHITE_PAWN <= removeType && removeType <= WHITE_KING)))
          return false;
      this->remove_by_pos(toX, toY);
      index = impl->findByPosition(fromX, fromY);
      log = "Delete|" + QString::number(removeType) + ","
              + QString::number(toX) + "," + QString::number(toY) + ";";
      if (removeType == BLACK_KING || removeType == WHITE_KING){
          QMessageBox msgBox;
          QString winner = (removeType == BLACK_KING) ? "White is winner" : "Black is winner";
          msgBox.setText("The End");
          msgBox.setInformativeText(winner);
          msgBox.exec();
      }
  }

  this->setData(createIndex(index, 0), QVariant(toX), Roles::PositionX);
  this->setData(createIndex(index, 0), QVariant(toY), Roles::PositionY);

  log += "Move|"+ QString::number(fromX) + "," + QString::number(fromY) + ","
          + QString::number(toX) + ","+ QString::number(toY) + "\n";
  this->saveList << log;
  queue = queue == 1 ? 2 : 1;

  return true;
}
