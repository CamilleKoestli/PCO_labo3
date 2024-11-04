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
    // TODO
    mutex.lock();
    bool requestSuccessful = false;

    if (stocks[what] - qty >= 0) {
        int bill = qty * getCostPerUnit(what);
        stocks[what] -= qty;

        money -= getEmployeeSalary(EmployeeType::Nurse);
        money += bill;

        interface->updateFund(uniqueId, money);
        requestSuccessful = true;
    }

    mutex.unlock();

    return requestSuccessful ? qty : 0;
}

void Hospital::freeHealedPatient() {
    //TODO
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
    //TODO
    int qty = 1;
    Seller* clinic = chooseRandomSeller(clinics);
    int bill = clinic->request(ItemType::PatientHealed, qty);

    if (bill > 0 && money >= bill && currentBeds + qty <= maxBeds) {
        mutex.lock();

        money -= bill;
        ++currentBeds;
        ++stocks[ItemType::PatientHealed];

        // Ajoute un patient avec un temps de 5 jours
        healedPatientsDaysLeft.push_back(5);

        mutex.unlock();
    }
}

int Hospital::send(ItemType it, int qty, int bill) {
    // TODO
    // Vérifie si l'hôpital a les ressources nécessaires pour traiter un patient
    if (maxBeds - currentBeds - qty >= 0 && money - bill >= 0) {
        mutex.lock();

        currentBeds += qty;
        stocks[it] += qty;

        money -= bill;
        money -= getEmployeeSalary(EmployeeType::Nurse);

        nbHospitalised++;
        mutex.unlock();
        return bill;
    }

    return 0;
}

void Hospital::run() {
    if (clinics.empty()) {
        std::cerr << "You have to assign clinics to a hospital before launching its routine" << std::endl;
        return;
    }

    interface->consoleAppendText(uniqueId, "[START] Hospital routine");

    // Boucle principale de la routine de l'hôpital
    while (!PcoThread::thisThread()->stopRequested()) {
        // Transfert des patients guéris des cliniques à l'hôpital
        transferPatientsFromClinic();

        // Libération des patients guéris ayant terminé leur convalescence
        freeHealedPatient();

        // Mise à jour de l'interface pour afficher les fonds et les stocks
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
