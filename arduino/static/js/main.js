function updateStatus() {
    fetch('/api/door-current')
        .then(response => response.json())
        .then(data => {
            const card = document.getElementById('status-card');
            const icon = document.getElementById('status-icon');
            const text = document.getElementById('status-text');
            const time = document.getElementById('status-time');

            const isOpen = data.durum === 'açık' || data.durum === 'acik';
            
            if (isOpen) {
                card.className = 'status-card open';
                icon.textContent = '🔓';
                text.textContent = 'KAPI AÇIK';
            } else {
                card.className = 'status-card closed';
                icon.textContent = '🔒';
                text.textContent = 'KAPI KAPALI';
            }
            time.textContent = 'Son güncelleme: ' + data.timestamp;
        })
        .catch(error => {
            document.getElementById('status-text').textContent = 'VERİ ALINAMIYOR';
            document.getElementById('status-text').style.color = '#888';
        });

    fetch('/api/door-history')
        .then(response => response.json())
        .then(data => {
            const historyList = document.getElementById('history-list');
            if (data.length === 0) {
                historyList.innerHTML = '<div class="no-data">Henüz kayıt bulunmuyor</div>';
                return;
            }
            
            historyList.innerHTML = data.slice().reverse().slice(0, 30).map(item => {
                const isOpen = item.durum === 'açık' || item.durum === 'acik';
                const statusClass = isOpen ? 'open-log' : 'closed-log';
                const statusIcon = isOpen ? '🔓' : '🔒';
                const statusText = isOpen ? 'AÇIK' : 'KAPALI';
                
                return `
                    <div class="history-item ${statusClass}">
                        <span class="history-status">${statusIcon} KAPI ${statusText}</span>
                        <span class="history-time">${item.timestamp}</span>
                    </div>
                `;
            }).join('');
        })
        .catch(error => {
            document.getElementById('history-list').innerHTML = 
                '<div class="no-data">Geçmiş veriler yüklenemedi</div>';
        });
}

// Sayfa yüklendiğinde başlat
updateStatus();
// Her 2 saniyede bir güncelle
setInterval(updateStatus, 2000);
