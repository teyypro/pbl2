#ifndef MEDICAL_RECORD_SERVICE_H
#define MEDICAL_RECORD_SERVICE_H

#include "MedicalRecordRepository.h"
#include "PrescriptionRepository.h"
#include "MedicineRepository.h"
#include "Helpers.h"
#include <ctime>

class MedicalRecordService {
private:
    MedicalRecordRepository recordRepo;
    PrescriptionRepository presRepo;
    MedicineRepository medRepo;

public:
    vector<MedicalRecord> getAll() { return recordRepo.findAll(); }
    vector<MedicalRecord> getByPatient(int patientId) { return recordRepo.findByPatient(patientId); }
    vector<MedicalRecord> getByDoctor(int doctorId) { return recordRepo.findByDoctor(doctorId); }
    MedicalRecord getById(int id) { return recordRepo.findById(id); }

    int createRecord(int patientId, int doctorId, int appointmentId,
                     const string& symptoms, const string& diagnosis,
                     const string& advice = "", const string& clinicalFindings = "") {
        MedicalRecord r;
        r.record_code = Helpers::generateCode("MR");
        r.patient_id = patientId;
        r.doctor_id = doctorId;
        r.appointment_id = appointmentId;

        // Lay ngay hien tai
        time_t now = time(nullptr);
        char buf[20];
        strftime(buf, sizeof(buf), "%Y-%m-%d", localtime(&now));
        r.visit_date = string(buf);

        r.symptoms = symptoms;
        r.diagnosis = diagnosis;
        r.advice = advice;
        r.clinical_findings = clinicalFindings;
        r.status = "draft";

        int id = recordRepo.create(r);
        if (id > 0) cout << "Tao phieu kham thanh cong! Ma: " << r.record_code << endl;
        return id;
    }

    bool finalizeRecord(int id) {
        MedicalRecord r = recordRepo.findById(id);
        if (r.id == 0) return false;
        r.status = "finalized";
        return recordRepo.update(r);
    }

    // Ke don thuoc cho phieu kham
    int createPrescription(int recordId, int patientId, int doctorId, const string& note = "") {
        Prescription p;
        p.prescription_code = Helpers::generateCode("RX");
        p.medical_record_id = recordId;
        p.patient_id = patientId;
        p.doctor_id = doctorId;

        time_t now = time(nullptr);
        char buf[20];
        strftime(buf, sizeof(buf), "%Y-%m-%d", localtime(&now));
        p.prescribed_at = string(buf);

        p.note = note;
        p.status = "pending";

        int id = presRepo.create(p);
        if (id > 0) cout << "Tao don thuoc thanh cong! Ma: " << p.prescription_code << endl;
        return id;
    }

    bool addMedicineToPrescription(int prescriptionId, int medicineId,
                                    const string& dosage, int quantity, int days,
                                    const string& frequency, bool morning, bool noon,
                                    bool evening, bool night) {
        Medicine med = medRepo.findById(medicineId);
        if (med.id == 0) { cout << "Thuoc khong ton tai!" << endl; return false; }

        PrescriptionDetail d;
        d.prescription_id = prescriptionId;
        d.medicine_id = medicineId;
        d.dosage = dosage;
        d.quantity = quantity;
        d.days = days;
        d.frequency = frequency;
        d.route = "oral";
        d.morning = morning;
        d.noon = noon;
        d.evening = evening;
        d.night = night;
        d.status = "active";

        int id = presRepo.addDetail(d);
        return id > 0;
    }

    Prescription getPrescription(int recordId) {
        return presRepo.findByMedicalRecord(recordId);
    }

    Prescription getPrescriptionById(int id) {
        return presRepo.findById(id);
    }
};

#endif
