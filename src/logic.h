#pragma once
#include <memory>
#include <QByteArray>
#include <QAbstractListModel>
#include <QQmlApplicationEngine>
#include <QVector>
#include <QFile>

struct Action
{
    QString name;
    QList <int> args;
};

class Logic: public QAbstractListModel
{
    Q_OBJECT
public:

    enum FiguresName{
        WHITE_PAWN,
        WHITE_KNIGHT,
        WHITE_ROOK,
        WHITE_BISHOP,
        WHITE_QUEEN,
        WHITE_KING,

        BLACK_PAWN,
        BLACK_KNIGHT,
        BLACK_ROOK,
        BLACK_BISHOP,
        BLACK_QUEEN,
        BLACK_KING,
    };

    enum GlobalConstants {
        BOARD_SIZE = 8
    };

    enum Roles {
        Type = Qt::UserRole,
        PositionX,
        PositionY,
    };
    
public:
    Q_PROPERTY(int boardSize READ boardSize CONSTANT)

    explicit                Logic(QObject *parent = 0);
                            ~Logic();
    int                     boardSize() const;
    Q_INVOKABLE void        clear();
    Q_INVOKABLE void        start();
    Q_INVOKABLE void        load();
    Q_INVOKABLE void        save();
    Q_INVOKABLE void        show_msg(const QString &);
    Q_INVOKABLE bool        prevStep();
    Q_INVOKABLE bool        nextStep();
    Q_INVOKABLE bool        move(int fromX, int fromY, int toX, int toY);

protected:
    int                     rowCount(const QModelIndex & parent) const override;
    QVariant                data(const QModelIndex & index, int role = Qt::DisplayRole) const override;
    bool                    setData(const QModelIndex &index, const QVariant &value, int role) override;
    bool                    removeRows(int position, int rows, const QModelIndex &parent) override;
    QHash<int, QByteArray>  roleNames() const override;
private:
    struct                  Impl;
    std::unique_ptr<Impl>   impl;
    QStringList             loadedFile;
    QStringList             saveList;
    int                     step;
    bool                    firstMove;

    bool                    pawns_move(int fromX, int fromY, int toX, int toY, int type);
    bool                    pawns_move_beat(int fromX, int fromY, int toX, int toY, int type);
    bool                    bishop_move(int fromX, int fromY, int toX, int toY, int type);
    bool                    rook_move(int fromX, int fromY, int toX, int toY, int type);
    bool                    knight_move(int fromX, int fromY, int toX, int toY, int type);
    bool                    king_move(int fromX, int fromY, int toX, int toY, int type);
    bool                    queen_move(int fromX, int fromY, int toX, int toY, int type);
    void                    remove_by_pos(int x, int y);
    QVector <Action>        parse_step(int step);
};


