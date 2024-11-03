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
    // TODO
    if (currentBeds < 0) {
        mutex.lock();
        currentBeds--;
        nbFree++;
        money -= getEmployeeSalary(EmployeeType::Nurse); // Paie l'infirmier pour chaque patient
        mutex.unlock();
        interface->updateFund(uniqueId, money);

        interface->consoleAppendText(uniqueId, "A healed patient has been discharged from the hospital.");
        interface->updateStock(uniqueId, &stocks);
    }
    
}

void Hospital::transferPatientsFromClinic() {
    // TODO
    for (auto &clinic: clinics) {
        int healedPatients = clinic->request(ItemType::PatientHealed, 1);

        if (healedPatients > 0) {
            mutex.lock();

            int nurseSalary = getEmployeeSalary(EmployeeType::Nurse);
            if (currentBeds < maxBeds) {
                currentBeds++;
                nbHospitalised++;
                money -= nurseSalary; 
                interface->updateFund(uniqueId, money);

                interface->consoleAppendText(uniqueId, "Transferred a healed patient from the clinic to the hospital.");
                interface->updateStock(uniqueId, &stocks);
                mutex.unlock();
                break; 
            } else {
                mutex.unlock(); 
                if (currentBeds >= maxBeds) {
                    interface->consoleAppendText(uniqueId, "No available beds for transferring a patient.");
                } else if (money < nurseSalary) {
                    interface->consoleAppendText(
                        uniqueId, "Insufficient funds to pay the nurse for transferring a patient.");
                }
            }
        }
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
