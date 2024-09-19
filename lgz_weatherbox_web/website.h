// HTML頭部
const char HTML[] = R"rawliteral(
<!DOCTYPE html>
<html lang="zh-TW">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>LGZ WeatherBox DashBoard</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            margin: 0;
            background-color: #f0f0f0;
        }
        .dashboard {
            background-color: white;
            padding: 20px;
            border-radius: 10px;
            box-shadow: 0 0 10px rgba(0,0,0,0.1);
        }
        .data-item {
            margin-bottom: 20px;
        }
        h1 {
            text-align: center;
            color: #333;
        }
    </style>
</head>
<body>
    <div class="dashboard">
        <h1>LGZ WeatherBox DashBoard</h1>
        <div class="data-item">
            <h2>Temperature: <span id="temperature">Loading...</span></h2>
        </div>
        <div class="data-item">
            <h2>Humidity: <span id="humidity">Loading...</span></h2>
        </div>
        <div class="data-item">
            <h2>PM2.5: <span id="pm25">Loading...</span></h2>
        </div>
    </div>

    <script src="https://cdnjs.cloudflare.com/ajax/libs/jquery/3.6.0/jquery.min.js"></script>
    <script>
        function updateData() {
            // 這裡應該是您的API端點
            $.ajax({
                url: '/api/data',
                method: 'GET',
                dataType: 'json',
                success: function(data) {
                    $('#temperature').text(data.temperature + ' °C');
                    $('#humidity').text(data.humidity + ' %');
                    $('#pm25').text(data.pm25 + ' µg/m³');
                },
                error: function() {
                    // alert('無法獲取數據，請稍後再試。');
                    const err = "Loading...";
                    $('#temperature').text(err + ' °C');
                    $('#humidity').text(err + ' %');
                    $('#pm25').text(err + ' µg/m³');
                }
            });
        }

        // 每60秒更新一次數據
        setInterval(updateData, 5000);

        // 頁面載入時立即更新數據
        updateData();
    </script>
</body>
</html>
)rawliteral";