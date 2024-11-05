#include "hospital.h"
#include "costs.h"
#include <iostream>
#include <pcosynchro/pcothread.h>

IWindowInterface *Hospital::interface = nullptr;

Hospital::Hospital(int uniqueId, int fund, int maxBeds)
    : Seller(fund, uniqueId), maxBeds(maxBeds), currentBeds(0),
      nbHospitalised(0), nbFree(0) {
    interface->updateFund(uniqueId, fund);
    interface->consoleAppendText(
        uniqueId, "Hospital Created with " + QString::number(maxBeds) + " beds");

    std::vector<ItemType> initialStocks = {
        ItemType::PatientHealed,
        ItemType::PatientSick
    };

    for (const auto &item: initialStocks) {
        stocks[item] = 0;
    }
}

int Hospital::request(ItemType what, int qty) {
    int bill = qty * getCostPerUnit(what);

    mutex.lock();
    if (stocks[what] >= qty) {
        stocks[what] -= qty;
        currentBeds -= qty;
        money += bill;

        interface->updateFund(uniqueId, money);
        mutex.unlock();
        return bill;
    }
    mutex.unlock();
    return 0;
}

void Hospital::freeHealedPatient() {
    mutex.lock();
    for (auto it = healedPatientsDaysLeft.begin(); it != healedPatientsDaysLeft.end(); ) {
        if (*it > 0) {
            --(*it);  // Décrémente les jours restants pour ce patient

            if (*it == 0) {
                // Le patient a terminé sa récupération et sort de l'hôpital
                --stocks[ItemType::PatientHealed];
                ++nbFree;       
                --currentBeds;  

                interface->consoleAppendText(uniqueId, "A healed patient leaves the hospital.");

                it = healedPatientsDaysLeft.erase(it);  // Supprime le patient de la deque
            } else {
                ++it; // Passe au patient suivant
            }
        }
    }
    mutex.unlock();
}

void Hospital::transferPatientsFromClinic() {
    int qty = 1;
    Seller* clinic = chooseRandomSeller(clinics);
    int bill = clinic->request(ItemType::PatientHealed, qty);

    mutex.lock();
    if (bill > 0 && money >= bill && currentBeds + qty <= maxBeds) {
        money -= bill;
        ++currentBeds;
        stocks[ItemType::PatientHealed] += qty;

        // Ajoute un patient avec un temps de 5 jours
        healedPatientsDaysLeft.push_back(5);

        interface->consoleAppendText(uniqueId, "Transferred a healed patient from the clinic.");
    }
    mutex.unlock();
}

int Hospital::send(ItemType it, int qty, int bill) {
    int nurseSalary = getEmployeeSalary(EmployeeType::Nurse);
    mutex.lock();
    if ((maxBeds - currentBeds >= qty) && (money >= bill + nurseSalary)) {

        currentBeds += qty;
        stocks[it] += qty;
        money -= bill;
        money -= nurseSalary;
        ++nbHospitalised;

        mutex.unlock();
        return bill;
    }
    mutex.unlock();
    return 0;
}

void Hospital::run() {
    if (clinics.empty()) {
        std::cerr << "You have to assign clinics to a hospital before launching its routine" << std::endl;
        return;
    }

    interface->consoleAppendText(uniqueId, "[START] Hospital routine");

    while (!PcoThread::thisThread()->stopRequested()) {
        // Transfert des patients guéris des cliniques à l'hôpital
        transferPatientsFromClinic();

        // Libération des patients guéris ayant terminé leur convalescence
        freeHealedPatient();

        interface->updateFund(uniqueId, money);
        interface->updateStock(uniqueId, &stocks);

        // Simule un délai d'attente
        interface->simulateWork();
    }

    interface->consoleAppendText(uniqueId, "[STOP] Hospital routine");
}

int Hospital::getAmountPaidToWorkers() {
    return nbHospitalised * getEmployeeSalary(EmployeeType::Nurse);
}

int Hospital::getNumberPatients() {
    return stocks[ItemType::PatientSick] + stocks[ItemType::PatientHealed] +
           nbFree;
}

std::map<ItemType, int> Hospital::getItemsForSale() { return stocks; }

void Hospital::setClinics(std::vector<Seller *> clinics) {
    this->clinics = clinics;

    for (Seller *clinic: clinics) {
        interface->setLink(uniqueId, clinic->getUniqueId());
    }
}

void Hospital::setInterface(IWindowInterface *windowInterface) {
    interface = windowInterface;
}
