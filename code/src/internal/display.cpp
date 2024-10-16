#include "display.h"

constexpr double SCENEWIDTH = 1000.0;
constexpr double SCENELENGTH = 1500.0;
constexpr double ELEMENT_WIDTH = 75.0;
constexpr double ELEMENT_WIDTH_BIG = 150.0;


static DisplayView* theDisplay;

SupplierItem::SupplierItem() = default;
ClinicItem::ClinicItem() = default;
HospitalItem::HospitalItem() = default;
ResourceItem::ResourceItem() = default;
ProductionItem::ProductionItem() = default;


void DisplayView::place_resources(int x, int y, int id, std::vector<bool> resources){

    QPixmap fund(QString("images/funds_color.png"));
    QPixmap _fund = fund.scaledToWidth(ELEMENT_WIDTH / 3);
    auto fund_item = new ResourceItem();

    int index = -1;
    int index2 = -3;
    fund_item->setPixmap(_fund);
    fund_item->setPos(x, y + -1 * (ELEMENT_WIDTH_BIG / 3));
    m_scene->addItem(fund_item);
    
    // Créer et positionner le texte pour les fonds
    funds[id] = new QGraphicsSimpleTextItem("Waiting...");
    funds[id]->setPos(x - 50 + 80, y + (index2 - 1) * (ELEMENT_WIDTH_BIG / 3 / 2) + 55);
    m_scene->addItem(funds[id]);

    if (resources[0]) {
        QPixmap petrol(QString("images/patientSick.png"));
        QPixmap _petrol = petrol.scaledToWidth(ELEMENT_WIDTH / 3);
        auto petrol_item = new ResourceItem();
        petrol_item->setPixmap(_petrol);
        petrol_item->setPos(x + (ELEMENT_WIDTH_BIG), y + index++ * (ELEMENT_WIDTH_BIG / 3 / 2));
        m_scene->addItem(petrol_item);
        
        // Créer et positionner le texte pour les patients malades
        patientsSick[id] = new QGraphicsSimpleTextItem("Waiting...");
        patientsSick[id]->setPos(x - 50 + (ELEMENT_WIDTH_BIG) + 80, y + index2++ * (ELEMENT_WIDTH_BIG / 3 / 2) + 50);
        m_scene->addItem(patientsSick[id]);
    }

    if (resources[1]) {
        QPixmap copper(QString("images/patientHealed.png"));
        QPixmap _copper = copper.scaledToWidth(ELEMENT_WIDTH / 3);
        auto copper_item = new ResourceItem();
        copper_item->setPixmap(_copper);
        copper_item->setPos(x + (ELEMENT_WIDTH_BIG), y + index++ * (ELEMENT_WIDTH_BIG / 3 / 2));
        m_scene->addItem(copper_item);
        
        // Créer et positionner le texte pour les patients guéris
        patientsHealed[id] = new QGraphicsSimpleTextItem("Waiting...");
        patientsHealed[id]->setPos(x - 50 + (ELEMENT_WIDTH_BIG) + 80, y + index2++ * (ELEMENT_WIDTH_BIG / 3 / 2) + 50);
        m_scene->addItem(patientsHealed[id]);
    }

    // Répéter pour les autres ressources
    if (resources[2]) {
        QPixmap chip(QString("images/syringe.png"));
        QPixmap _chip = chip.scaledToWidth(ELEMENT_WIDTH / 3);
        auto chip_item = new ResourceItem();
        chip_item->setPixmap(_chip);
        chip_item->setPos(x + (ELEMENT_WIDTH_BIG), y + index++ * (ELEMENT_WIDTH_BIG / 3 / 2));
        m_scene->addItem(chip_item);
        
        syringes[id] = new QGraphicsSimpleTextItem("Waiting...");
        syringes[id]->setPos(x - 50 + (ELEMENT_WIDTH_BIG) + 80, y + index2++ * (ELEMENT_WIDTH_BIG / 3 / 2) + 50);
        m_scene->addItem(syringes[id]);
    }

    if (resources[3]) {
        QPixmap sand(QString("images/pill.png"));
        QPixmap _sand = sand.scaledToWidth(ELEMENT_WIDTH / 3);
        auto sand_item = new ResourceItem();
        sand_item->setPixmap(_sand);
        sand_item->setPos(x + (ELEMENT_WIDTH_BIG), y + index++ * (ELEMENT_WIDTH_BIG / 3 / 2));
        m_scene->addItem(sand_item);
        
        pills[id] = new QGraphicsSimpleTextItem("Waiting...");
        pills[id]->setPos(x - 50 + (ELEMENT_WIDTH_BIG) + 80, y + index2++ * (ELEMENT_WIDTH_BIG / 3 / 2) + 50);
        m_scene->addItem(pills[id]);
    }

    if (resources[4]) {
        QPixmap robot(QString("images/scalpel.png"));
        QPixmap _robot = robot.scaledToWidth(ELEMENT_WIDTH / 3);
        auto robot_item = new ResourceItem();
        robot_item->setPixmap(_robot);
        robot_item->setPos(x + (ELEMENT_WIDTH_BIG), y + index++ * (ELEMENT_WIDTH_BIG / 3 / 2));
        m_scene->addItem(robot_item);
        
        scalpels[id] = new QGraphicsSimpleTextItem("Waiting...");
        scalpels[id]->setPos(x - 50 + (ELEMENT_WIDTH_BIG) + 80, y + index2++ * (ELEMENT_WIDTH_BIG / 3 / 2) + 50);
        m_scene->addItem(scalpels[id]);
    }

    if (resources[5]) {
        QPixmap plastic(QString("images/thermometer.png"));
        QPixmap _plastic = plastic.scaledToWidth(ELEMENT_WIDTH / 3);
        auto plastic_item = new ResourceItem();
        plastic_item->setPixmap(_plastic);
        plastic_item->setPos(x + (ELEMENT_WIDTH_BIG), y + index++ * (ELEMENT_WIDTH_BIG / 3 / 2));
        m_scene->addItem(plastic_item);
        
        thermometers[id] = new QGraphicsSimpleTextItem("Waiting...");
        thermometers[id]->setPos(x - 50 + (ELEMENT_WIDTH_BIG) + 80, y + index2++ * (ELEMENT_WIDTH_BIG / 3 / 2) + 50);
        m_scene->addItem(thermometers[id]);
    }

    if (resources[6]) {
        QPixmap plastic(QString("images/stethoscope.png"));
        QPixmap _plastic = plastic.scaledToWidth(ELEMENT_WIDTH / 3);
        auto plastic_item = new ResourceItem();
        plastic_item->setPixmap(_plastic);
        plastic_item->setPos(x + (ELEMENT_WIDTH_BIG), y + index++ * (ELEMENT_WIDTH_BIG / 3 / 2));
        m_scene->addItem(plastic_item);
        
        stethoscopes[id] = new QGraphicsSimpleTextItem("Waiting...");
        stethoscopes[id]->setPos(x - 50 + (ELEMENT_WIDTH_BIG) + 80, y + index2++ * (ELEMENT_WIDTH_BIG / 3 / 2) + 50);
        m_scene->addItem(stethoscopes[id]);
    }
}


DisplayView::DisplayView(unsigned int nbSuppliers, unsigned int nbClinics, unsigned int nbHospitals, QWidget *parent) :
    QGraphicsView(parent)
{
    theDisplay = this;

    m_scene = new QGraphicsScene(this);
    createResourceAssociations(nbSuppliers, nbHospitals, nbClinics);

    funds = std::vector<QGraphicsSimpleTextItem*>(nbSuppliers + nbClinics + nbHospitals);
    syringes = std::vector<QGraphicsSimpleTextItem*>(nbSuppliers + nbClinics + nbHospitals);
    pills = std::vector<QGraphicsSimpleTextItem*>(nbSuppliers + nbClinics + nbHospitals);
    scalpels = std::vector<QGraphicsSimpleTextItem*>(nbSuppliers + nbClinics + nbHospitals);
    thermometers = std::vector<QGraphicsSimpleTextItem*>(nbSuppliers + nbClinics + nbHospitals);
    stethoscopes = std::vector<QGraphicsSimpleTextItem*>(nbSuppliers + nbClinics + nbHospitals);
    patientsSick = std::vector<QGraphicsSimpleTextItem*>(nbSuppliers + nbClinics + nbHospitals);
    patientsHealed = std::vector<QGraphicsSimpleTextItem*>(nbSuppliers + nbClinics + nbHospitals);

    this->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    // this->setMinimumHeight(SCENEWIDTH);
    // this->setMinimumWidth(SCENELENGTH);
    this->setScene(m_scene);
    this->setMaximumHeight(SCENEWIDTH);
    this->setMaximumWidth(SCENELENGTH);

    for (unsigned int i = 0; i < nbSuppliers; ++i){
        QString str;
        switch(i % 3){
            case 0:
                str = "images/ambulance.png";
                break;
            case 1:
                str = "images/medicalDevice.png";
                break;
            case 2:
                str = "images/pharmacy.png";
                break;
        }

        QPixmap img(str);
        QPixmap supplierPixmap;
        int x = 0 + SCENELENGTH / 6;//QRandomGenerator::system()->bounded(SCENELENGTH / 3);
        int y = (SCENEWIDTH / nbSuppliers) * i + SCENEWIDTH / nbSuppliers / 2;

        supplierPixmap = img.scaledToWidth(ELEMENT_WIDTH_BIG);
        auto supplier = new ProductionItem();
        supplier->setPixmap(supplierPixmap);
        supplier->setPos(x, y);
        m_scene->addItem(supplier);
        m_productItem.push_back(supplier);

        place_resources(x, y, i, resourceAssociations[i]);
    }

    for (unsigned int i = 0; i < nbHospitals; ++i) {
        QPixmap img(QString("images/hospital.png"));
        QPixmap hospitalPixmap;
        int x = (SCENELENGTH / 3) + (SCENELENGTH / 6);
//        int x = (SCENELENGTH / 3) * 2 + (SCENELENGTH / 6);//QRandomGenerator::system()->bounded(SCENELENGTH / 3);
        int y = (SCENEWIDTH / nbHospitals) * i + SCENEWIDTH / nbHospitals / 2;

        hospitalPixmap = img.scaledToWidth(ELEMENT_WIDTH_BIG);
        auto hospital = new ProductionItem();
        hospital->setPixmap(hospitalPixmap);

        hospital->setPos(x, y);
        m_scene->addItem(hospital);
        m_productItem.push_back(hospital);

        place_resources(x, y, i + nbSuppliers, resourceAssociations[i + nbSuppliers]);
    }

    for (unsigned int i = 0; i < nbClinics; ++i) {
        QPixmap img(QString("images/clinic.png"));
        QPixmap clinicPixmap;
        int x = (SCENELENGTH / 3) * 2 + (SCENELENGTH / 6);
//        int x = (SCENELENGTH / 3) + (SCENELENGTH / 6);//QRandomGenerator::system()->bounded(SCENELENGTH / 3);
        int y = (SCENEWIDTH / nbClinics) * i + SCENEWIDTH / nbClinics / 2;

        clinicPixmap = img.scaledToWidth(ELEMENT_WIDTH_BIG);
        auto clinic = new ProductionItem();
        clinic->setPixmap(clinicPixmap);
        clinic->setPos(x, y);
        m_scene->addItem(clinic);
        m_productItem.push_back(clinic);

        place_resources(x, y, i + nbSuppliers + nbHospitals, resourceAssociations[i + nbHospitals + nbSuppliers]);

    }
}

void DisplayView::createResourceAssociations(unsigned int nbSuppliers, unsigned int nbHospitals, unsigned int nbClinics) {
    unsigned int totalEntities = nbSuppliers + nbHospitals + nbClinics;
    resourceAssociations.resize(totalEntities, std::vector<bool>(7, false));



    // Remplissez le vecteur resourceAssociations en fonction du nombre d'entités de chaque type
    for (unsigned int i = 0; i < nbSuppliers; ++i) {
        std::vector<bool> supplierResources(7, false);
        if (i % 3 == 0) {
            supplierResources[0] = true; // Ambulance
        } else if (i % 3 == 1) {
            supplierResources[4] = true; // MedicalDevice
            supplierResources[5] = true;
            supplierResources[6] = true;
        } else if (i % 3 == 2) {
            supplierResources[2] = true; // Pharmacie
            supplierResources[3] = true;
        }
        resourceAssociations[i] = supplierResources;
    }

    for (unsigned int i = 0; i < nbHospitals; ++i) {
        // Pour les grossistes, toutes les ressources sont associées (true)
        std::vector<bool> hospitalResources(7, false);

        hospitalResources[0] = true;
        hospitalResources[1] = true;

        resourceAssociations[i+nbSuppliers] = hospitalResources;
    }

    for (unsigned int i = 0; i < nbClinics; ++i) {
        std::vector<bool> clinicResources(7, false);
        if (i % 3 == 0) {
            clinicResources[0] = true; // Clinic 1
            clinicResources[1] = true;
            clinicResources[3] = true;
            clinicResources[5] = true;
        } else if (i % 3 == 1) {
            clinicResources[0] = true; // Clinic 2
            clinicResources[1] = true; 
            clinicResources[2] = true;
            clinicResources[6] = true;
        } else if (i % 3 == 2) {
            clinicResources[0] = true; // Clinic 3
            clinicResources[1] = true; 
            clinicResources[3] = true;
            clinicResources[4] = true;
        }
        resourceAssociations[i + nbSuppliers + nbHospitals] = clinicResources;
    }

}

void DisplayView::update_fund(int idx, QString fund) {
    this->funds[idx]->setText(fund);
}

void DisplayView::set_link(int from, int to) {
    QPoint pFrom(m_productItem[from]->pos().x(), m_productItem[from]->pos().y());
    QPoint pTo(m_productItem[to]->pos().x(), m_productItem[to]->pos().y());
    QLine line;
    QColor linkColor;

    if (pFrom.x() >  SCENELENGTH / 2){
        /* Link Clinic -> Hospital and suppliers*/
        line = QLine(QPoint(pFrom.x(), pFrom.y() + (m_productItem[from]->pixmap().height() / 2)),
                     QPoint(pTo.x() + ((ELEMENT_WIDTH) * 2) + 50, pTo.y() + (m_productItem[to]->pixmap().height() / 2)));
        
        if(pTo.x() < SCENELENGTH / 2)
            linkColor = QColor(0, 255, 0);
        else
            linkColor = QColor(0, 200, 0);
    } else {
        if (pFrom.x() < SCENELENGTH / 2) {
            line = QLine(QPoint(pTo.x(), pTo.y() + (m_productItem[from]->pixmap().height()/2)),
                         QPoint(pFrom.x() + ((ELEMENT_WIDTH)*2) + 50, pFrom.y() + (m_productItem[to]->pixmap().height()/2)));

            linkColor = QColor(238,130,238);
        } else {
            line = QLine(QPoint(pTo.x(), pTo.y() + (m_productItem[from]->pixmap().height() / 2) + 10),
                     QPoint(pFrom.x() + ((ELEMENT_WIDTH) * 2) + 50, pFrom.y() + (m_productItem[to]->pixmap().height() / 2) + 10));
            linkColor = QColor(255,128,0);
        }
    }

    QPen pen(linkColor);
    pen.setWidth(2);

    m_scene->addLine(line, pen);
}

void DisplayView::update_stocks(int idx, std::map<ItemType, int>* stocks) {

    std::vector<bool> updates = resourceAssociations[idx];

    if(updates[0]){
        this->patientsSick[idx]->setText(QString::number((*stocks)[ItemType::PatientSick]));
    }
    if(updates[1]){
        this->patientsHealed[idx]->setText(QString::number((*stocks)[ItemType::PatientHealed]));
    }
    if(updates[2]){
        this->syringes[idx]->setText(QString::number((*stocks)[ItemType::Syringe]));
    }
    if(updates[3]){
        this->pills[idx]->setText(QString::number((*stocks)[ItemType::Pill]));
    }
    if(updates[4]){
        this->scalpels[idx]->setText(QString::number((*stocks)[ItemType::Scalpel]));
    }
    if(updates[5]){
        this->thermometers[idx]->setText(QString::number((*stocks)[ItemType::Thermometer]));
    }
    if(updates[6]){
        this->stethoscopes[idx]->setText(QString::number((*stocks)[ItemType::Stethoscope]));
    }
}
