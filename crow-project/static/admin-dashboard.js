/* ==========================================================================
   AROGYA ADMIN DASHBOARD - DYNAMIC BACKEND LINKING SCRIPT
   ========================================================================== */

document.addEventListener('DOMContentLoaded', () => {
    // 1. Fetch and Populate Live Doctor & Personnel Data
    fetchDoctorsData();

    // 2. Fetch and Populate Live System Beds & Triage Stats
    fetchSystemStats();

    // 3. Live Search Filter for Data Table
    const searchInput = document.querySelector('.search-bar input');
    if (searchInput) {
        searchInput.addEventListener('input', (e) => {
            const query = e.target.value.toLowerCase().trim();
            const tableRows = document.querySelectorAll('.data-table tbody tr');

            tableRows.forEach(row => {
                const textContent = row.textContent.toLowerCase();
                row.style.display = textContent.includes(query) ? '' : 'none';
            });
        });
    }

    // 4. Action Buttons & Modals
    const addStaffBtn = document.querySelector('.banner-actions .btn-primary');
    if (addStaffBtn) {
        addStaffBtn.addEventListener('click', () => {
            const name = prompt("Enter Full Name for New Staff/Doctor:");
            const email = prompt("Enter Email:");
            const password = prompt("Enter Temporary Password:");
            const role = prompt("Enter Role (Doctor / Nurse / Admin / Staff):", "Doctor");

            if (name && email && password && role) {
                registerNewUser(name, email, password, role);
            }
        });
    }

    // Notification Bell Toggle
    const notificationBell = document.querySelector('.header-actions .icon-btn');
    if (notificationBell) {
        notificationBell.addEventListener('click', () => {
            const dot = notificationBell.querySelector('.notification-dot');
            if (dot) dot.style.display = 'none';
            alert('Arogya System Alerts:\n• Live backend connection active.\n• Check server logs for active socket connections.');
        });
    }
});

// Async function to fetch doctor list from C++ Crow backend
async function fetchDoctorsData() {
    try {
        const response = await fetch('/api/doctors/list');
        if (!response.ok) throw new Error('Failed to fetch doctors list');
        
        const doctors = await response.json();
        const tbody = document.querySelector('.data-table tbody');
        if (!tbody) return;

        tbody.innerHTML = ''; // Clear hardcoded rows

        if (doctors.length === 0) {
            tbody.innerHTML = '<tr><td colspan="5" style="text-align: center; color: var(--text-muted);">No doctors currently registered in system.</td></tr>';
            return;
        }

        doctors.forEach(doc => {
            const initials = doc.name.split(' ').map(n => n[0]).join('').substring(0, 2).toUpperCase();
            
            // Map status text to CSS badge class
            let statusClass = 'status-active';
            let statusText = doc.status || 'On Duty';
            if (statusText.toLowerCase().includes('busy')) statusClass = 'status-busy';
            if (statusText.toLowerCase().includes('off')) statusClass = 'status-off';

            const tr = document.createElement('tr');
            tr.innerHTML = `
                <td>
                    <div class="user-cell">
                        <div class="avatar-sm">${initials}</div>
                        <div>
                            <strong>${doc.name}</strong>
                            <span class="sub">${doc.specialization || 'Specialist'}</span>
                        </div>
                    </div>
                </td>
                <td>${doc.department}</td>
                <td>${doc.shift || '08:00 AM – 04:00 PM'}</td>
                <td><span class="status-pill ${statusClass}">${statusText}</span></td>
                <td><button class="btn-icon" title="Manage Options">&bull;&bull;&bull;</button></td>
            `;
            tbody.appendChild(tr);
        });

    } catch (error) {
        console.error('Error loading doctors:', error);
    }
}

// Async function to fetch system beds and capacity overview
async function fetchSystemStats() {
    try {
        const response = await fetch('/api/beds/status');
        if (!response.ok) throw new Error('Failed to fetch bed status');
        
        const stats = await response.json();
        
        // Update ICU & Bed Occupancy Card if elements match
        const occupancyCardValue = document.querySelector('.metrics-grid .metric-card:nth-child(3) .metric-value');
        if (occupancyCardValue && stats.total_beds > 0) {
            const occupancyRate = Math.round((stats.occupied_beds / stats.total_beds) * 100);
            occupancyCardValue.textContent = `${occupancyRate}%`;
            
            const footerText = document.querySelector('.metrics-grid .metric-card:nth-child(3) .metric-footer span:nth-child(2)');
            if (footerText) {
                footerText.textContent = `${stats.available_beds} Beds Remaining`;
            }
        }
    } catch (error) {
        console.error('Error loading bed metrics:', error);
    }
}

// Helper to register staff via backend API
async function registerNewUser(name, email, password, role) {
    try {
        const response = await fetch('/api/admin/create-user', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ name, email, password, role })
        });
        const result = await response.json();
        if (response.ok) {
            alert(result.message || 'User registered successfully!');
            fetchDoctorsData(); // Refresh list
        } else {
            alert('Error: ' + (result.message || 'Could not create account'));
        }
    } catch (err) {
        console.error('Registration network error:', err);
    }
}
