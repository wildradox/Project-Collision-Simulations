# PROJECT COLLISION dengan Raylib C++
Projek dibuat oleh Jeff Rehobot H L G - 5024241073
Simulasi deteksi tabrakan bola 2D menggunakan tiga algoritma berbeda: Brute Force, QuadTree, dan Hybrid.
Program dibuat menggunakan Raylib (C++), dan menampilkan visualisasi tabrakan fisika secara real-time dengan ratusan bola yang bergerak bebas.

# Tujuan Proyek
Proyek ini bertujuan untuk:
1. Membandingkan performa tiga metode collision detection.
2. Menunjukkan bagaimana optimasi spatial partitioning (QuadTree) meningkatkan performa.
3. Mendemonstrasikan simulasi fisika seperti tabrakan elastis, anti-stuck, dan bouncing.
4. Memberikan simulasi interaktif yang dapat diubah mode-nya secara real-time.

# Fitur Utama
1. Tiga Mode Collision Detection
- Mode	Deskripsi	Kompleksitas
- Brute Force	Cek semua bola satu per satu	O(n²)
- QuadTree	Optimasi dengan membagi ruang menjadi node kecil	O(n log n)
- Hybrid	QuadTree (broad phase) + Brute Force lokal (narrow phase)	cepat & akurat

Mode dapat diganti secara real-time:
B → Brute Force
Q → QuadTree
T → Hybrid

2. Sistem Fisika
- Gerakan bola bebas
- Tabrakan elastis antar bola
- Pantulan dengan dinding
- Memastikan bola tidak “nempel” atau bergetar saat overlap

3. HUD (Info Panel)
Panel informasi di sisi kiri (toggle dengan H) menampilkan:
- Mode algoritma yang aktif
- Jumlah bola
- FPS
- Collision checks per frame
- Average speed bola
- Status simulasi (Running / Paused)

4. Visual Debug
- Menampilkan struktur QuadTree Grid (toggle G)
- Membantu melihat bagaimana pembagian ruang bekerja

Program ini hanya memerlukan raylib dan compiler C++ seperti g++ atau clang++.

# Penjelasan Algoritma
Brute Force
Setiap bola dibandingkan dengan semua bola lainnya.
Paling sederhana, tapi paling lambat untuk jumlah besar.

QuadTree
Ruang dibagi menjadi node-node kecil.
Bola hanya dibandingkan dengan bola dalam area terdekat.
Lebih cepat dan efisien.

Hybrid
QuadTree untuk mencari kelompok bola terdekat
Dalam satu cluster: brute force tugas kecil
Memberikan keseimbangan antara kecepatan dan akurasi.

# Screenshot
<img width="1393" height="920" alt="image" src="https://github.com/user-attachments/assets/dda51ff5-7e8a-4d03-b11e-eec978c4b98a" />

# Kesimpulan
Proyek ini berhasil menunjukkan perbedaan performa dan karakteristik antara Brute Force, QuadTree, dan Hybrid.
QuadTree memberikan peningkatan performa signifikan pada jumlah objek besar, sementara Hybrid memberi keseimbangan terbaik antara efisiensi dan akurasi. Program ini juga menampilkan struktur QuadTree secara real-time, sehingga memudahkan pemahaman konsep spatial partitioning.

# Lisensi
Silakan gunakan, modifikasi, atau mengembangkan projek ini secara bebas.
