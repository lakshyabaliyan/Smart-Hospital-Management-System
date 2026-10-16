/* AROGYA Patient Dashboard interactions (plain JavaScript) */
(() => {
  const sidebar = document.getElementById('sidebar');
  const menuToggle = document.getElementById('menuToggle');
  const sidebarScrim = document.getElementById('sidebarScrim');
  const profileToggle = document.getElementById('profileToggle');
  const profileMenu = document.getElementById('profileMenu');
  const profileControl = document.getElementById('profileControl');

  function setSidebar(open) {
    sidebar.classList.toggle('is-open', open);
    sidebarScrim.hidden = !open;
    menuToggle.setAttribute('aria-label', open ? 'Close navigation' : 'Open navigation');
    document.body.classList.toggle('sidebar-open', open);
  }

  function setProfileMenu(open) {
    profileMenu.hidden = !open;
    profileToggle.setAttribute('aria-expanded', String(open));
  }

  function navigate(label) {
    document.querySelectorAll('.nav-item').forEach((item) => {
      const selected = item.dataset.nav === label;
      item.classList.toggle('active', selected);
      if (selected) item.setAttribute('aria-current', 'page');
      else item.removeAttribute('aria-current');
    });
    setSidebar(false);
    setProfileMenu(false);
    if (label === 'Profile') {
      setProfileMenu(true);
      return;
    }
    const targetIds = {
      'Overview': 'overview',
      'Appointments': 'appointments',
      'Queue Status': 'queue-status',
      'Medical Records': 'medical-records',
      'Billing': 'billing'
    };
    const target = document.getElementById(targetIds[label]);
    if (target) target.scrollIntoView({ behavior: 'smooth', block: 'start' });
    else if (label === 'Appointments') window.alert('This is a visual demo. Appointment booking is not connected to a hospital system.');
  }

  menuToggle.addEventListener('click', () => setSidebar(!sidebar.classList.contains('is-open')));
  sidebarScrim.addEventListener('click', () => setSidebar(false));
  profileToggle.addEventListener('click', () => setProfileMenu(profileMenu.hidden));

  document.addEventListener('click', (event) => {
    if (!profileControl.contains(event.target)) setProfileMenu(false);
  });

  document.querySelectorAll('[data-nav]').forEach((button) => {
    button.addEventListener('click', () => {
      const label = button.dataset.nav;
      if (label === 'Profile' && button.closest('.profile-menu')) {
        setProfileMenu(false);
        window.alert('Profile details are demo-only and are not connected to a patient database.');
        return;
      }
      navigate(label);
    });
  });

  document.getElementById('signOutDemo').addEventListener('click', () => {
    setProfileMenu(false);
    window.alert('This is a demonstration dashboard. Sign-out is not connected to an account.');
  });

  document.querySelectorAll('[data-download]').forEach((button) => {
    button.addEventListener('click', () => {
      const title = button.dataset.download;
      const content = `${title}\n\nAROGYA Patient Portal - Demonstration only\nThis is sample information, not an actual medical record.`;
      const file = new Blob([content], { type: 'text/plain;charset=utf-8' });
      const url = URL.createObjectURL(file);
      const link = document.createElement('a');
      link.href = url;
      link.download = title.toLowerCase().replace(/[^a-z0-9]+/g, '-') + '-demo.txt';
      document.body.appendChild(link);
      link.click();
      link.remove();
      URL.revokeObjectURL(url);
    });
  });

  // Keep the highlighted sidebar item in sync when the user scrolls.
  const sectionMap = [
    ['overview', 'Overview'],
    ['appointments', 'Appointments'],
    ['queue-status', 'Queue Status'],
    ['medical-records', 'Medical Records'],
    ['billing', 'Billing']
  ];
  let ticking = false;
  window.addEventListener('scroll', () => {
    if (ticking) return;
    ticking = true;
    window.requestAnimationFrame(() => {
      const anchor = window.scrollY + 150;
      let current = 'Overview';
      sectionMap.forEach(([id, label]) => {
        const section = document.getElementById(id);
        if (section && section.offsetTop <= anchor) current = label;
      });
      document.querySelectorAll('.nav-item').forEach((item) => {
        const selected = item.dataset.nav === current;
        item.classList.toggle('active', selected);
        if (selected) item.setAttribute('aria-current', 'page');
        else item.removeAttribute('aria-current');
      });
      ticking = false;
    });
  }, { passive: true });
})();
