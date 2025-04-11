# Tucil2_13523159

# 📦 Quadtree Image Compression

## 📌 Deskripsi Singkat

Program ini mengimplementasikan **algoritma kompresi gambar menggunakan struktur data Quadtree**. Gambar dibagi secara rekursif menjadi blok-blok hingga memenuhi kriteria tertentu berdasarkan nilai error, seperti:

- **Variance**
- **Mean Absolute Deviation (MAD)**
- **Max Pixel Difference**
- **Entropy**

Kompresi dilakukan dengan cara mengganti blok yang homogen (berdasarkan error threshold) dengan warna rata-rata blok tersebut. Program juga mendukung **rekonstruksi gambar hasil kompresi**, di mana setiap leaf node terdalam direpresentasikan sebagai **satu piksel**, memungkinkan visualisasi yang efisien terhadap hasil segmentasi.

## 💻 Requirement dan Instalasi

### Tools
- **Compiler C++**

## ⚙️ Cara Kompilasi

Pastikan struktur folder seperti berikut:

```

## ▶️ Cara Menjalankan dan Menggunakan Program

1. Jalankan executable:

```bash
./bin/main.exe
```

2. Program akan menampilkan prompt interaktif:

```
Masukkan path gambar input (dengan ekstensi)
Path: test/image1.png

Masukkan path gambar output
Path: output/result.png

Pilih metode error:
(Variance, Mean Absolute Deviation (MAD), Max Pixel Difference, Entropy)

Metode error: Variance

Masukkan nilai threshold: 500

Masukkan ukuran blok minimum: 8
```

3. Program akan memproses gambar dan menyimpan hasilnya.

## 📷 Output

- Gambar hasil kompresi disimpan dalam path output yang kamu masukkan.

## 👤 Author

**Nama**: Anas Ghazi Al Gifari
**NIM**: 13523159
**Email**: anasghassie75@gmail.com  
**Proyek**: Kompresi Gambar Menggunakan Quadtree  
**Tahun**: 2025
