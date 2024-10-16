#ifndef DISPLAY_H
#define DISPLAY_H

#include <QGraphicsView>
#include <QGraphicsItem>
#include <pcosynchro/pcosemaphore.h>
#include <QGraphicsSimpleTextItem>
#include <QLine>
#include <QPen>

#include "seller.h"

class ResourceItem : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT
        Q_PROPERTY(QPointF pos READ pos WRITE setPos)
    public:
        ResourceItem();
        PcoSemaphore sem;
};

class SupplierItem : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT
        Q_PROPERTY(QPointF pos READ pos WRITE setPos)
    public:
        SupplierItem();
        PcoSemaphore sem;
};

class ClinicItem : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT
        Q_PROPERTY(QPointF pos READ pos WRITE setPos)
    public:
        ClinicItem();
        PcoSemaphore sem;
};

class HospitalItem : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT
        Q_PROPERTY(QPointF pos READ pos WRITE setPos)
    public:
        HospitalItem();
        PcoSemaphore sem;
};

class ProductionItem : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT
        Q_PROPERTY(QPointF pos READ pos WRITE setPos)
    public:
        ProductionItem();
        PcoSemaphore sem;
};

class DisplayView : public QGraphicsView
{
    Q_OBJECT
public:
    DisplayView(unsigned int nbSuppliers, unsigned int nbClinic, unsigned int nbHospitals, QWidget *parent);
    //~DisplayView();

    std::vector<QGraphicsSimpleTextItem*> funds;
    std::vector<QGraphicsSimpleTextItem*> syringes;
    std::vector<QGraphicsSimpleTextItem*> pills;
    std::vector<QGraphicsSimpleTextItem*> scalpels;
    std::vector<QGraphicsSimpleTextItem*> thermometers;
    std::vector<QGraphicsSimpleTextItem*> stethoscopes;
    std::vector<QGraphicsSimpleTextItem*> patientsSick;
    std::vector<QGraphicsSimpleTextItem*> patientsHealed;

    std::vector<ProductionItem*> m_productItem;


    void update_stocks(int idx, std::map<ItemType, int>* stocks);
    void update_fund(int idx, QString fund);

    void set_link(int from, int to);

private:
    QGraphicsScene *m_scene;

    std::vector<std::vector<bool>> resourceAssociations;

    void place_resources(int x, int y, int id, std::vector<bool> resources);
    void createResourceAssociations(unsigned int nbSuppliers, unsigned int nbHospitals, unsigned int nbClinic);
public slots:

};

#endif // DISPLAY_H
