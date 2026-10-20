/* ==========================================================================
   AROGYA ADMIN DASHBOARD - INTERACTIVITY SCRIPT
   ========================================================================== */

document.addEventListener('DOMContentLoaded', () => {
    // 1. Live Search Filter for Data Table (Staff & Doctors)
    const searchInput = document.querySelector('.search-bar input');
    const tableRows = document.querySelectorAll('.data-table tbody tr');

    if (searchInput) {
        searchInput.addEventListener('input', (e) => {
            const query = e.target.value.toLowerCase().trim();

            tableRows.forEach(row => {
                const textContent = row.textContent.toLowerCase();
                if (textContent.includes(query)) {
                    row.style.display = '';
                } else {
                    row.style.display = 'none';
                }
            });
        });
    }

    // 2. Interactive Action Buttons ("Add New Staff/Doctor" & "Assign Trauma Specialist")
    const addStaffBtn = document.querySelector('.banner-actions .btn-primary');
    if (addStaffBtn) {
        addStaffBtn.addEventListener('click', () => {
            alert('Opening Arogya Staff Registration & Role Assignment Portal...');
        });
    }

    const assignTraumaBtn = document.querySelector('.emergency-card .btn-dark');
    if (assignTraumaBtn) {
        assignTraumaBtn.addEventListener('click', () => {
            alert('Trauma Response Protocol initiated. Dispatching notification to on-call surgeon team for Patient #AG-8821.');
        });
    }

    // 3. Row Action Ellipsis (`...`) interactive popups
    const actionButtons = document.querySelectorAll('.btn-icon');
    actionButtons.forEach((btn, index) => {
        btn.addEventListener('click', () => {
            const row = btn.closest('tr');
            const doctorName = row.querySelector('strong').textContent;
            const action = prompt(`Manage actions for ${doctorName}:\n1. Edit Shift Timing\n2. Reassign Department\n3. Set Status to Off Duty\n\nEnter choice number (1-3):`, "1");
            
            if (action === "1") {
                const newTiming = prompt("Enter new shift hours (e.g., 09:00 AM – 05:00 PM):");
                if (newTiming) {
                    row.cells[2].textContent = newTiming;
                    alert(`Shift updated successfully for ${doctorName}.`);
                }
            } else if (action === "3") {
                const statusPill = row.querySelector('.status-pill');
                statusPill.className = 'status-pill status-off';
                statusPill.textContent = 'Off Duty';
                alert(`${doctorName} status updated to Off Duty.`);
            }
        });
    });

    // 4. Notification Bell Interactive Toggle
    const notificationBell = document.querySelector('.header-actions .icon-btn');
    if (notificationBell) {
        notificationBell.addEventListener('click', () => {
            const dot = notificationBell.querySelector('.notification-dot');
            if (dot) {
                dot.style.display = 'none'; // Clear notification badge on click
            }
            alert('Arogya System Alerts:\n• Bed occupancy reached 84% in General ICU.\n• Priority 1 Triage alert active for ER Bed 02.');
        });
    }
});