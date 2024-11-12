<?php
header('Content-Type: application/json');
include 'config.php';

// Rata-rata suhu dari data keseluruhan
$sql = "SELECT AVG(suhu) AS avg_temp FROM tb_cuaca";
$result = $conn->query($sql);
$suhurata = round($result->fetch_assoc()['avg_temp'], 2);

// Nilai maksimum dan minimum suhu
$sql = "SELECT MAX(suhu) AS suhumax, MIN(suhu) AS suhumin FROM tb_cuaca";
$result = $conn->query($sql);
$row = $result->fetch_assoc();
$suhumax = $row['suhumax'];
$suhumin = $row['suhumin'];

// Data suhu dan kelembapan maksimum beserta detail
$sql = "SELECT * FROM tb_cuaca 
        WHERE suhu = (SELECT MAX(suhu) FROM tb_cuaca) 
        AND humid = (SELECT MAX(humid) FROM tb_cuaca)";
$result = $conn->query($sql);

$nilai_suhu_max_humid_max = [];
while($row = $result->fetch_assoc()) {
    $nilai_suhu_max_humid_max[] = array(
        "id" => $row['id'],
        "suhu" => $row['suhu'],
        "humid" => $row['humid'],
        "lux" => $row['lux'],
        "ts" => $row['ts']
    );
}

// Mendapatkan bulan dan tahun untuk data suhu atau kelembapan maksimum
$sql = "SELECT DATE_FORMAT(ts, '%m-%y') AS bulan, suhu, humid 
        FROM tb_cuaca 
        WHERE suhu = (SELECT MAX(suhu) FROM tb_cuaca) 
        OR humid = (SELECT MAX(humid) FROM tb_cuaca) 
        ORDER BY suhu DESC, humid DESC 
        LIMIT 2";
$result = $conn->query($sql);

$month_year_max = [];
while($row = $result->fetch_assoc()) {
    $month_year_max[] = array(
        "bulan" => $row['bulan']
    );
}

// Membuat response JSON sesuai format yang diminta
$response = array(
    "suhumax" => $suhumax,
    "suhumin" => $suhumin,
    "suhurata" => $suhurata,
    "nilai_suhu_max_humid_max" => $nilai_suhu_max_humid_max,
    "month_year_max" => $month_year_max
);

// Mengembalikan JSON
echo json_encode($response);

// Tutup koneksi
$conn->close();
?>
