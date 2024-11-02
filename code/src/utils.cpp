#include "utils.h"
#include <iostream>


void Utils::endService() {
    // TODO
     for (const auto &thread: threads) {
        thread->requestStop();
    }
    std::cout << "The hospital is closed !" << std::endl;
}

void Utils::externalEndService() {
    endService();
    semEnd.acquire();
    utilsThread->join();
}

std::vector<Ambulance*> createAmbulances(int nbAmbulances, int idStart){
    if (nbAmbulances < 1){
        qInfo() << "Cannot make the programm work with less than 1 Supplier";
        exit(-1);
    }

    std::vector<Ambulance*> ambulances;

    for(int i = 0; i < nbAmbulances; ++i){
        switch(i % 3) {

            case 0:{
                std::map<ItemType, int> initialAmbulanceStock = {{ItemType::PatientSick, INITIAL_PATIENT_SICK}};
                ambulances.push_back(new Ambulance(i + idStart, SUPPLIER_FUND, {ItemType::PatientSick}, initialAmbulanceStock));
                break;
            }
        }
    }
    return ambulances;
}

std::vector<Supplier*> createSuppliers(int nbSuppliers, int idStart) {
    if (nbSuppliers < 1){
        qInfo() << "Cannot make the programm work with less than 1 Supplier";
        exit(-1);
    }

    std::vector<Supplier*> suppliers;

    for(int i = 0; i < nbSuppliers; ++i){
        switch(i % 3) {
            case 1:{
                suppliers.push_back(new MedicalDeviceSupplier(i + idStart, SUPPLIER_FUND));
                break;
            }
            case 2:{
                suppliers.push_back(new Pharmacy(i + idStart, SUPPLIER_FUND));
                break;
            }
        }
    }
    return suppliers;
}

std::vector<Clinic*> createClinics(int nbClinics, int idStart) {
    if (nbClinics < 1){
        qInfo() << "Cannot make the programm work with less than 1 Clinic";
        exit(-1);
    }

    std::vector<Clinic*> clinics;

    for(int i = 0; i < nbClinics; ++i) {
        switch(i % 3) {
            case 0:
                clinics.push_back(new Pulmonology(i + idStart, CLINICS_FUND));
                break;

            case 1:
                clinics.push_back(new Cardiology(i + idStart, CLINICS_FUND));
                break;

            case 2:
                clinics.push_back(new Neurology(i + idStart, CLINICS_FUND));
                break;
        }
    }


    return clinics;
}

std::vector<Hospital*> createHospitals(int nbHospital, int idStart) {
    if(nbHospital < 1){
        qInfo() << "Cannot launch the programm without any hospitalr";
        exit(-1);
    }
    std::vector<Hospital*> hospitals;

    for(int i = 0; i < nbHospital; ++i){
        hospitals.push_back(new Hospital(i + idStart, HOSPITALS_FUND, MAX_BEDS_PER_HOSTPITAL));
    }

    return hospitals;
}


Utils::Utils(int nbSupplier, int nbClinic, int nbHospital) {
    int nbAmbulances = nbSupplier / 3;
    if (nbSupplier % 3 != 0) {
        nbAmbulances += 1;
    }
    this->ambulances.resize(nbAmbulances);
    this->suppliers.resize(nbSupplier - nbAmbulances);
    this->hospitals.resize(nbHospital);
    this->clinics.resize(nbClinic);

    this->ambulances = createAmbulances(nbSupplier, 0);
    this->suppliers = createSuppliers(nbSupplier, 0);
    this->hospitals = createHospitals(nbHospital, nbSupplier);
    this->clinics = createClinics(nbClinic, nbSupplier + nbHospital);

    int clinicsByHospital = nbClinic / nbHospital;
    int clinicsShared = nbClinic % nbHospital;

    int countSupplier = 0;
    int countClinic = 0;

    std::vector<Seller*> tmpHospitals;
    std::vector<Seller*> tmpSuppliers;
    std::vector<Seller*> tmpClinics;
    
    // Préparation des hopitaux, ils ont besoin des clincs
    for(auto& h : hospitals) {
        std::vector<Seller*> tmpClinics(clinics.begin() + countClinic, clinics.begin() + countClinic + clinicsByHospital);
        tmpClinics.insert(tmpClinics.end(), clinics.end() - clinicsShared, clinics.end());

        countClinic += clinicsByHospital;
        
        h->setClinics(tmpClinics);
        tmpHospitals.push_back(static_cast<Seller*>(h));
    }

    // Préparation des ambulances, ils ont besoin des hôpitaux
    for(auto& a : ambulances){
        a->setHospitals(tmpHospitals);
    }

    for(auto& s : suppliers){
        tmpSuppliers.push_back(static_cast<Seller*>(s));
    }

    // Préparation des clincs, qui ont besoin des hôpitaux et des suppliers
    for(auto& c : clinics) {
        c->setHospitalsAndSuppliers(tmpHospitals, tmpSuppliers);
    }

    utilsThread = std::make_unique<PcoThread>(&Utils::run, this);
}

void Utils::run() {

    for(size_t i = 0; i < ambulances.size(); ++i) {
        threads.emplace_back(std::make_unique<PcoThread>(&Ambulance::run, ambulances[i]));
    }

    for(size_t i = 0; i < suppliers.size(); ++i) {
        threads.emplace_back(std::make_unique<PcoThread>(&Supplier::run, suppliers[i]));
    }

    for(size_t i = 0; i < clinics.size(); ++i) {
        threads.emplace_back(std::make_unique<PcoThread>(&Clinic::run, clinics[i]));
    }

    for(size_t i = 0; i < hospitals.size(); ++i) {
        threads.emplace_back(std::make_unique<PcoThread>(&Hospital::run, hospitals[i]));
    }

    for (auto& thread : threads) {
        thread->join();
    }
    
    int startPatient = INITIAL_PATIENT_SICK;

    int endPatient = 0;



    int startFund = (SUPPLIER_FUND * int(ambulances.size())) +
                    (SUPPLIER_FUND * int(suppliers.size())) +
                    (CLINICS_FUND * int(clinics.size())) +
                    (HOSPITALS_FUND * int(hospitals.size()));

    int endFund = 0;

    for (Ambulance* ambulance: ambulances) {
        endFund += ambulance->getFund();
        endFund += ambulance->getAmountPaidToWorkers();
        endPatient += ambulance->getNumberPatients();
    }

    for (Supplier* supplier: suppliers) {
        endFund += supplier->getFund();
        endFund += supplier->getAmountPaidToWorkers();
    }

    for (Clinic* clinic: clinics) {
        endFund += clinic->getFund();
        endFund += clinic->getAmountPaidToWorkers();
        endPatient += clinic->getNumberPatients();
    }

    for (Hospital* hospital: hospitals) {
        endFund += hospital->getFund();
        endFund += hospital->getAmountPaidToWorkers();
        endPatient += hospital->getNumberPatients();
    }

    finalReport = QString("The expected fund is : %1 and you got at the end : %2\n").arg(startFund).arg(endFund);
    finalReport += QString("The expected patient is : %1 and you got at the end : %2").arg(startPatient).arg(endPatient);

    qInfo() << "The expected fund is : " << startFund << " and you got at the end : " << endFund;
    semEnd.release();
}

QString Utils::getFinalReport()
{
    return finalReport;
}
