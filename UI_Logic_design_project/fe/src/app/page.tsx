"use client";

import { ref, child, get, update } from "firebase/database";
import database from "./firebase";
import { useState, useEffect } from "react";
import Image from "next/image";
import "./app.css";
import ColorPicker from "@/componetsWeb/selectColor";

export default function Home() {
    const [data, setData] = useState<{ temp: number; humi: number } | undefined>();
    const [fadeIn, setFadeIn] = useState(false);
    const [stateLed, setStateLed] = useState(false);
    const [selectedColor, setSelectedColor] = useState<number[]>([255, 255, 255]); // Mảng RGB mặc định là trắng
    const [showNotification, setShowNotification] = useState(false); // Trạng thái hiển thị thông báo
    const [progress, setProgress] = useState(100); // Tiến độ thanh chạy thời gian
    const [isLoading, setIsLoading] = useState(false); // Trạng thái loading

    useEffect(() => {
        const fetchData = async () => {
            const dbRef = ref(database);
            try {
                const snapshot = await get(child(dbRef, `Sensor`));
                if (snapshot.exists()) {
                    setData(snapshot.val());
                    setFadeIn(true); // Trigger the animation
                    setTimeout(() => setFadeIn(false), 1500); // Reset after animation duration
                } else {
                    console.log("No data available");
                }
            } catch (error) {
                console.error(error);
            }
        };

        fetchData(); // Initial fetch on component mount
        const interval = setInterval(fetchData, 10000);

        return () => clearInterval(interval);
    }, []);

    const handleLedToggle = async () => {
        setIsLoading(true); // Bắt đầu loading
        try {
            const newState = !stateLed;
            const dbRef = ref(database, `LED/state`);
            await update(dbRef.parent!, { state: newState });

            // Trì hoãn 2 giây trước khi thay đổi trạng thái
            setTimeout(() => {
                setStateLed(newState);
                setIsLoading(false); // Kết thúc loading sau 2 giây
            }, 3000);

            console.log(`LED state updated to: ${newState}`);
        } catch (error) {
            console.error("Failed to update LED state:", error.message || error);
            setIsLoading(false); // Đảm bảo kết thúc loading trong trường hợp lỗi
        }
    };

    const handleColorSubmit = async () => {
        try {
            const dbRef = ref(database, `LED/color`);
            const colorObject = { r: selectedColor[0], g: selectedColor[1], b: selectedColor[2] };

            await update(dbRef.parent!, { color: colorObject });

            console.log("Color sent to Firebase:", colorObject);

            // Hiển thị thông báo và bắt đầu thanh chạy
            setShowNotification(true);
            setProgress(100);

            const interval = setInterval(() => {
                setProgress((prev) => {
                    if (prev <= 0) {
                        clearInterval(interval);
                        setShowNotification(false); // Ẩn thông báo khi kết thúc
                        return 0;
                    }
                    return prev - 5; // Giảm dần tiến độ
                });
            }, 100);
        } catch (error) {
            console.error("Failed to update color:", error.message || error);
        }
    };

    return (
        <div
            className="flex min-h-screen flex-col lg:flex-row justify-center items-center xl:h-screen gap-32"
            style={{
                backgroundImage: `url('/img/background.jpg')`,
                backgroundSize: "cover",
                backgroundPosition: "center",
            }}
        >
            <div className="flex flex-col">
                <button
                    className={`font-semibold rounded-lg transition-all duration-300 ease-in-out transform mb-8 h-16 mt-10 ${
                        isLoading
                            ? "bg-gray-400 cursor-not-allowed" // Hiệu ứng khi loading
                            : stateLed
                            ? "bg-green-500 text-white hover:bg-green-600 hover:scale-105"
                            : "bg-red-500 text-white hover:bg-red-600 hover:scale-105"
                    }`}
                    onClick={handleLedToggle}
                    disabled={isLoading} // Vô hiệu hóa nút khi loading
                >
                    {isLoading ? (
                        <span className="flex items-center justify-center gap-2">
                            <svg
                                className="animate-spin h-5 w-5 text-white"
                                xmlns="http://www.w3.org/2000/svg"
                                fill="none"
                                viewBox="0 0 24 24"
                            >
                                <circle
                                    className="opacity-25"
                                    cx="12"
                                    cy="12"
                                    r="10"
                                    stroke="currentColor"
                                    strokeWidth="4"
                                ></circle>
                                <path
                                    className="opacity-75"
                                    fill="currentColor"
                                    d="M4 12a8 8 0 018-8V0C6.477 0 2 4.477 2 10h2zm2 5.291A7.962 7.962 0 014 12H0c0 2.21.895 4.21 2.343 5.657l1.657-1.366z"
                                ></path>
                            </svg>
                            Đang xử lý...
                        </span>
                    ) : (
                        <>{stateLed ? "LED ON" : "LED OFF"}</>
                    )}
                </button>

                <div className="w-80">
                    <ColorPicker onColorChange={(rgbArray: number[]) => setSelectedColor(rgbArray)} />
                </div>

                <button
                    onClick={handleColorSubmit}
                    className="bg-blue-500 text-white text-xl font-semibold rounded-lg h-16 mt-7 hover:bg-blue-600 transition-all"
                >
                    Chuyển đổi
                </button>

                {showNotification && (
                    <div
                        className={`fixed inset-0 flex justify-center items-center z-50 transition-opacity duration-300`}
                        style={{
                            backgroundColor: "rgba(0, 0, 0, 0.5)", // Màu nền mờ để tạo hiệu ứng overlay
                        }}
                    >
                        <div
                            className="bg-white text-gray-800 rounded-lg p-6 shadow-lg flex flex-col items-center text-center animate-fade-in-out"
                            style={{ width: "300px" }}
                        >
                            <p className="text-lg font-medium mb-2">Đang gửi yêu cầu ...</p>
                            <div
                                className="bg-gray-300 rounded-full h-3 w-full overflow-hidden"
                                style={{ marginTop: "8px" }}
                            >
                                <div
                                    className="bg-blue-500 h-full rounded-full transition-all"
                                    style={{
                                        width: `${progress}%`,
                                    }}
                                ></div>
                            </div>
                        </div>
                    </div>
                )}
            </div>

            <div
                className={`relative 2xl:w-[30%] xl:w-[35%] lg:w-[40%] md:w-[45%] sm:w-[50%] w-[80%] h-[600px] lg:mb-0 mb-10 rounded-3xl overflow-hidden border-sky-100 transition-transform duration-1000`}
            >
                <div className="absolute inset-0 bg-slate-200 opacity-35"></div>

                <div className="relative z-10 items-center h-full">
                    <h1 className="pt-12 font-bold text-center text-3xl text-white">Nhiệt độ và độ ẩm</h1>
                    <div className="flex flex-col justify-center items-center pt-14">
                        <Image src={"/img/cloud.png"} alt="cloud" width={200} height={200} className="" />

                        <p className="font-bold text-5xl text-white relative pt-6 animate-typewriter">
                            {data ? data.temp.toFixed(2) : "27.34"}
                            <span className="before_temp text-base text-white font-semibold">o</span>
                            <span className="after_temp text-xl text-white font-semibold">C</span>
                        </p>

                        <p className="font-medium text-2xl text-white pt-4 animate-typewriter">
                            Nhiệt độ tương đối ổn định
                        </p>

                        <div className="flex flex-row gap-7 pt-8 items-center">
                            <Image src={"/img/water-regular-72.png"} alt="humidity" width={80} height={120} />
                            <div>
                                <p className="font-semibold text-3xl text-white animate-typewriter">
                                    {data ? data.humi.toFixed(2) : "30.45"} <span className="text-xl">%</span>
                                </p>
                                <p className="font-normal text-xl text-white">Humidity</p>
                            </div>
                        </div>
                    </div>
                </div>
            </div>
        </div>
    );
}
