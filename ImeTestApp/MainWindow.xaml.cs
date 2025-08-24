using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Runtime.InteropServices;
using System.Diagnostics;
using System.Windows.Interop;
using System.Collections;

namespace ImeTestApp
{
    /// <summary>
    /// MainWindow.xaml の相互作用ロジック
    /// </summary>
    public partial class MainWindow : Window
    {
        [DllImport("EventHooker.dll")]
        private static extern void InstallHook();

        [DllImport("EventHooker.dll")]
        private static extern void UninstallHook();

        const int WM_COPYDATA = 0x004A;

        [StructLayout(LayoutKind.Sequential)]
        public struct COPYDATASTRUCT
        {
            public IntPtr dwData;
            public int    cbData;
            public IntPtr lpData;
        }

        public MainWindow()
        {
            InitializeComponent();

            this.Loaded += (s, e) =>
            {
                InstallHook();
            };

            this.Closed += (s, e) =>
            {
                UninstallHook();
            };

            var hWnd = new WindowInteropHelper(this).EnsureHandle();
            HwndSource source = HwndSource.FromHwnd(hWnd);
            source.AddHook(new HwndSourceHook(WndProc));
        }

        private IntPtr WndProc(IntPtr hWnd, int msg, IntPtr wParam, IntPtr lParam, ref bool handled)
        {
            if(msg == WM_COPYDATA)
            {
                COPYDATASTRUCT cds = (COPYDATASTRUCT)Marshal.PtrToStructure(lParam, typeof(COPYDATASTRUCT));
                string received = Marshal.PtrToStringUni(cds.lpData);
                DisplayBox.Text = received;
            }

            return IntPtr.Zero;
        }

        private void Log(string message)
        {
            Trace.WriteLine("$$$" + message);
        }
    }
}
