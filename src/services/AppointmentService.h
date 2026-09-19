#ifndef APPOINTMENT_SERVICE_H
#define APPOINTMENT_SERVICE_H

#include "AppointmentRepository.h"
#include "PatientRepository.h"
#include "DoctorRepository.h"
#include "DepartmentRepository.h"
#include "ServiceRepository.h"
#include "Helpers.h"

class AppointmentService {
private:
    AppointmentRepository aptRepo;
    PatientRepository patientRepo;
    DoctorRepository doctorRepo;
    DepartmentRepository deptRepo;
    ServiceRepository svcRepo;

public:
    vector<Appointment> getAll() { return aptRepo.findAll(); }
    vector<Appointment> getByPatient(int patientId) { return aptRepo.findByPatient(patientId); }
    vector<Appointment> getByDoctor(int doctorId) { return aptRepo.findByDoctor(doctorId); }
    Appointment getById(int id) { return aptRepo.findById(id); }

    int createAppointment(int patientId, int doctorId, int departmentId,
                          int serviceId, const string& scheduledAt,
                          int createdBy, const string& note = "") {
        // Validate benh nhan
        Patient p = patientRepo.findById(patientId);
        if (p.id == 0) { cout << "Benh nhan khong ton tai!" << endl; return -1; }

        // Validate bac si
        Doctor d = doctorRepo.findById(doctorId);
        if (d.id == 0) { cout << "Bac si khong ton tai!" << endl; return -1; }

        Appointment apt;
        apt.appointment_code = Helpers::generateCode("APT");
        apt.patient_id = patientId;
        apt.doctor_id = doctorId;
        apt.department_id = departmentId;
        apt.service_id = serviceId;
        apt.scheduled_at = scheduledAt;
        apt.status = "pending";
        apt.created_by = createdBy;
        apt.note = note;

        int id = aptRepo.create(apt);
        if (id > 0) {
            cout << "Dat lich thanh cong! Ma: " << apt.appointment_code << endl;
        }
        return id;
    }

    bool cancelAppointment(int id, const string& reason) {
        Appointment a = aptRepo.findById(id);
        if (a.id == 0) { cout << "Lich hen khong ton tai!" << endl; return false; }
        if (a.status == "completed" || a.status == "cancelled") {
            cout << "Khong the huy lich hen nay!" << endl;
            return false;
        }
        return aptRepo.updateStatus(id, "cancelled", reason);
    }

    bool confirmAppointment(int id) {
        return aptRepo.updateStatus(id, "confirmed");
    }

    bool completeAppointment(int id) {
        return aptRepo.updateStatus(id, "completed");
    }
};

#endif
