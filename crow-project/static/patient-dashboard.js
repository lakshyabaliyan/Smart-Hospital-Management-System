/* AROGYA Patient Dashboard interactions */
(() => {
  const sidebar = document.getElementById("sidebar");
  const menuToggle = document.getElementById("menuToggle");
  const sidebarScrim = document.getElementById("sidebarScrim");
  const profileToggle = document.getElementById("profileToggle");
  const profileMenu = document.getElementById("profileMenu");
  const profileControl = document.getElementById("profileControl");
  const signOutButton = document.getElementById("signOutDemo");

  async function loadPatientSession() {
    try {
      const response = await fetch("/api/session");

      if (!response.ok) {
        window.location.replace("/signin.html");
        return;
      }

      const user = await response.json();

      if (user.role !== "Patient") {
        window.location.replace("/signin.html");
        return;
      }

      const displayName = user.name || user.email;
      const firstName = displayName.trim().split(/\s+/)[0];
      const initials = displayName
        .trim()
        .split(/\s+/)
        .slice(0, 2)
        .map((part) => part.charAt(0))
        .join("")
        .toUpperCase();

      document.querySelector(".profile-name strong").textContent = displayName;
      document.querySelector(".profile-name small").textContent = user.email;
      document.querySelector(".avatar").textContent = initials;
      document.querySelector(".welcome h1 em").textContent = `${firstName}.`;

      if (signOutButton) {
        signOutButton.textContent = "Sign out";
      }
    } catch (error) {
      window.location.replace("/signin.html");
    }
  }

  loadPatientSession();

  function setSidebar(open) {
    sidebar.classList.toggle("is-open", open);
    sidebarScrim.hidden = !open;
    menuToggle.setAttribute(
      "aria-label",
      open ? "Close navigation" : "Open navigation"
    );
    document.body.classList.toggle("sidebar-open", open);
  }

  function setProfileMenu(open) {
    profileMenu.hidden = !open;
    profileToggle.setAttribute("aria-expanded", String(open));
  }

  function navigate(label) {
    document.querySelectorAll(".nav-item").forEach((item) => {
      const selected = item.dataset.nav === label;
      item.classList.toggle("active", selected);

      if (selected) item.setAttribute("aria-current", "page");
      else item.removeAttribute("aria-current");
    });

    setSidebar(false);
    setProfileMenu(false);

    if (label === "Profile") {
      setProfileMenu(true);
      return;
    }

    const targetIds = {
      Overview: "overview",
      Appointments: "appointments",
      "Queue Status": "queue-status",
      "Medical Records": "medical-records",
      Billing: "billing"
    };

    const target = document.getElementById(targetIds[label]);
    if (target) target.scrollIntoView({ behavior: "smooth", block: "start" });
  }

  menuToggle.addEventListener("click", () => {
    setSidebar(!sidebar.classList.contains("is-open"));
  });

  sidebarScrim.addEventListener("click", () => setSidebar(false));

  profileToggle.addEventListener("click", () => {
    setProfileMenu(profileMenu.hidden);
  });

  document.addEventListener("click", (event) => {
    if (!profileControl.contains(event.target)) {
      setProfileMenu(false);
    }
  });

  document.querySelectorAll("[data-nav]").forEach((button) => {
    button.addEventListener("click", () => {
      const label = button.dataset.nav;

      if (label === "Profile" && button.closest(".profile-menu")) {
        setProfileMenu(false);
        window.alert("Profile details are still sample data in this dashboard.");
        return;
      }

      navigate(label);
    });
  });

  signOutButton.addEventListener("click", async () => {
    setProfileMenu(false);

    try {
      await fetch("/api/logout", { method: "POST" });
    } finally {
      window.location.replace("/signin.html");
    }
  });

  document.querySelectorAll("[data-download]").forEach((button) => {
    button.addEventListener("click", () => {
      const title = button.dataset.download;
      const content =
        `${title}\n\nAROGYA Patient Portal - Demonstration only\n` +
        "This is sample information, not an actual medical record.";

      const file = new Blob([content], { type: "text/plain;charset=utf-8" });
      const url = URL.createObjectURL(file);
      const link = document.createElement("a");

      link.href = url;
      link.download =
        title.toLowerCase().replace(/[^a-z0-9]+/g, "-") + "-demo.txt";

      document.body.appendChild(link);
      link.click();
      link.remove();
      URL.revokeObjectURL(url);
    });
  });

  const sectionMap = [
    ["overview", "Overview"],
    ["appointments", "Appointments"],
    ["queue-status", "Queue Status"],
    ["medical-records", "Medical Records"],
    ["billing", "Billing"]
  ];

  let ticking = false;

  window.addEventListener(
    "scroll",
    () => {
      if (ticking) return;
      ticking = true;

      window.requestAnimationFrame(() => {
        const anchor = window.scrollY + 150;
        let current = "Overview";

        sectionMap.forEach(([id, label]) => {
          const section = document.getElementById(id);
          if (section && section.offsetTop <= anchor) current = label;
        });

        document.querySelectorAll(".nav-item").forEach((item) => {
          const selected = item.dataset.nav === current;
          item.classList.toggle("active", selected);

          if (selected) item.setAttribute("aria-current", "page");
          else item.removeAttribute("aria-current");
        });

        ticking = false;
      });
    },
    { passive: true }
  );
})();