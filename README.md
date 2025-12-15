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
# Brute Force

Pada metode Brute Force, setiap bola dibandingkan dengan semua bola lainnya untuk mendeteksi tabrakan. Di dalam kode, hal ini dilakukan menggunakan dua perulangan bersarang, di mana bola ke-i akan dibandingkan dengan bola ke-(i+1) hingga bola terakhir. Untuk setiap pasangan bola, program menghitung jarak antar pusat bola dan menentukan apakah jarak tersebut lebih kecil dari jumlah radius kedua bola. Jika terjadi tabrakan, fungsi ResolveCollision dipanggil untuk memperbaiki posisi dan kecepatan bola.

Pendekatan ini sangat sederhana dan mudah dipahami karena tidak menggunakan struktur data tambahan. Namun, jumlah pengecekan tabrakan meningkat sangat cepat ketika jumlah bola bertambah. Dengan banyak bola, metode ini menghasilkan sangat banyak collision check dalam satu frame, sehingga menyebabkan FPS turun. Sehingga, Brute Force dianjurkan untuk simulasi dengan jumlah objek yang kecil.

# QuadTree

Pada metode QuadTree, ruang simulasi dibagi menjadi beberapa area berbentuk persegi panjang. Setiap bola dimasukkan ke dalam QuadTree berdasarkan posisi koordinatnya. Jika jumlah bola dalam satu node melebihi kapasitas tertentu, node tersebut akan dibagi menjadi empat bagian yang lebih kecil. Proses ini dilakukan setiap frame ketika mode QuadTree aktif.

Setelah QuadTree terbentuk, deteksi tabrakan tidak lagi dilakukan dengan membandingkan semua bola secara global. Untuk setiap bola, program hanya melakukan pencarian (Query) pada area di sekitar bola tersebut untuk mendapatkan daftar bola yang berdekatan. Hanya bola-bola dalam hasil query inilah yang kemudian dicek jaraknya dan diproses tabrakannya. Selain itu, kode memastikan bahwa setiap pasangan bola hanya diproses satu kali dengan membandingkan ID bola.

Dengan membatasi pengecekan hanya pada bola yang lokasinya berdekatan, jumlah collision check berkurang drastis dibandingkan Brute Force. Hal ini membuat performa simulasi jauh lebih baik ketika jumlah bola besar, terutama jika bola tersebar merata di dalam ruang.

# Hybrid

Metode Hybrid merupakan gabungan antara QuadTree dan Brute Force. Pada metode ini, QuadTree digunakan sebagai tahap awal untuk mengelompokkan bola-bola yang berada dalam area tertentu. Setelah mendapatkan kelompok bola melalui proses query QuadTree, program melakukan brute force secara lokal di dalam kelompok tersebut.

Pada implementasinya, hasil query diurutkan berdasarkan ID bola agar setiap pasangan bola hanya diproses satu kali. Brute force lokal kemudian digunakan untuk menyelesaikan tabrakan antar bola dalam kelompok kecil tersebut menggunakan fungsi ResolveCollision. Dengan cara ini, metode Hybrid tetap efisien karena ruang pencarian dibatasi oleh QuadTree, namun tetap akurat karena brute force digunakan pada skala kecil.

Metode Hybrid memberikan keseimbangan terbaik antara performa dan kestabilan simulasi, terutama pada kondisi kepadatan bola yang tinggi. Jumlah collision check lebih kecil dibandingkan Brute Force, dan lebih stabil dibandingkan QuadTree murni pada area yang sangat padat.

# Lampiran
Screenshot program
<img width="1387" height="858" alt="Screenshot 2025-12-15 104018" src="https://github.com/user-attachments/assets/6fa8ca18-0b40-4c8e-a1e4-818077d1669e" />
Video
![VideoObjectCollision-ezgif com-gif-maker](https://github.com/user-attachments/assets/84155ede-033c-4fb2-a2d4-a4525f5bc817)


# Kesimpulan
Proyek ini berhasil menunjukkan perbedaan performa dan karakteristik antara Brute Force, QuadTree, dan Hybrid.
QuadTree memberikan peningkatan performa signifikan pada jumlah objek besar, sementara Hybrid memberi keseimbangan terbaik antara efisiensi dan akurasi. Program ini juga menampilkan struktur QuadTree secara real-time, sehingga memudahkan pemahaman konsep spatial partitioning.

# Lisensi
Silakan gunakan, modifikasi, atau mengembangkan projek ini secara bebas.
