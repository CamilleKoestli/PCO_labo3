#include "clinic.h"
#include "costs.h"
#include <pcosynchro/pcothread.h>
#include <iostream>

IWindowInterface *Clinic::interface = nullptr;

Clinic::Clinic(int uniqueId, int fund, std::vector<ItemType> resourcesNeeded)
    : Seller(fund, uniqueId), nbTreated(0), resourcesNeeded(resourcesNeeded)
{
    interface->updateFund(uniqueId, fund);
    interface->consoleAppendText(uniqueId, "Factory created");

    for (const auto &item : resourcesNeeded)
    {
        stocks[item] = 0;
    }
}

bool Clinic::verifyResources()
{
    for (auto item : resourcesNeeded)
    {
        if (stocks[item] == 0)
        {
            return false;
        }
    }
    return true;
}

int Clinic::request(ItemType what, int qty)
{
    // TODO à checker
    // Vérifie que le type de patient demandé est malade
    if (what == ItemType::PatientSick){
        return qty;
    }
    return 0;
}

void Clinic::treatPatient()
{
    // TODO a checker

    if (verifyResources())
    {
        mutex.lock();
        stocks[ItemType::PatientSick]--;
        stocks[ItemType::PatientHealed]++;
        nbTreated++;
        mutex.unlock();

        // Temps simulant un traitement
        interface->simulateWork();

        // TODO
        interface->consoleAppendText(uniqueId, "Clinic have healed a patient");
        
        interface->updateStock(uniqueId, &stocks);
    }
}

void Clinic::orderResources()
{
    // TODO a checker
    for (const auto& item : resourcesNeeded) {
        
        // Si la ressource manquant
        if (stocks[item] == 0) {  
            Seller* supplier = chooseRandomSeller(suppliers);
            int cost = supplier->request(item, 1);  
            
            if (cost > 0) {
                mutex.lock();
                stocks[item]++;  
                money -= cost;  
                mutex.unlock();
                interface->updateFund(uniqueId, money);
                interface->consoleAppendText(uniqueId, "Ordered resources from supplier");
            } else {
                interface->consoleAppendText(uniqueId, "Failed to order resources");
            }
        }
    }
}

void Clinic::run()
{
    if (hospitals.empty() || suppliers.empty())
    {
        std::cerr << "You have to give to hospitals and suppliers to run a clinic" << std::endl;
        return;
    }
    interface->consoleAppendText(uniqueId, "[START] Factory routine");

    while (PcoThread::thisThread()->stopRequested()/*true TODO*/)
    {

        if (verifyResources())
        {
            treatPatient();
        }
        else
        {
            orderResources();
        }

        interface->simulateWork();

        interface->updateFund(uniqueId, money);
        interface->updateStock(uniqueId, &stocks);
    }
    interface->consoleAppendText(uniqueId, "[STOP] Factory routine");
}

void Clinic::setHospitalsAndSuppliers(std::vector<Seller *> hospitals, std::vector<Seller *> suppliers)
{
    this->hospitals = hospitals;
    this->suppliers = suppliers;

    for (Seller *hospital : hospitals)
    {
        interface->setLink(uniqueId, hospital->getUniqueId());
    }
    for (Seller *supplier : suppliers)
    {
        interface->setLink(uniqueId, supplier->getUniqueId());
    }
}

int Clinic::getTreatmentCost()
{
    return 0;
}

int Clinic::getWaitingPatients()
{
    return stocks[ItemType::PatientSick];
}

int Clinic::getNumberPatients()
{
    return stocks[ItemType::PatientSick] + stocks[ItemType::PatientHealed];
}

int Clinic::send(ItemType it, int qty, int bill)
{
    return 0;
}

int Clinic::getAmountPaidToWorkers()
{
    return nbTreated * getEmployeeSalary(getEmployeeThatProduces(ItemType::PatientHealed));
}

void Clinic::setInterface(IWindowInterface *windowInterface)
{
    interface = windowInterface;
}

std::map<ItemType, int> Clinic::getItemsForSale()
{
    return stocks;
}

Pulmonology::Pulmonology(int uniqueId, int fund) : Clinic::Clinic(uniqueId, fund, {ItemType::PatientSick, ItemType::Pill, ItemType::Thermometer}) {}

Cardiology::Cardiology(int uniqueId, int fund) : Clinic::Clinic(uniqueId, fund, {ItemType::PatientSick, ItemType::Syringe, ItemType::Stethoscope}) {}

Neurology::Neurology(int uniqueId, int fund) : Clinic::Clinic(uniqueId, fund, {ItemType::PatientSick, ItemType::Pill, ItemType::Scalpel}) {}
