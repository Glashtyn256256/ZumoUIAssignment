using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO.Ports;

namespace ZumoUIAssignment
{
    public partial class Form1 : Form
    {
        SerialPort port; public Form1()
        {
            InitializeComponent(); this.FormClosed += new FormClosedEventHandler(Form1_FormClosed); if (port == null)
            {
                //Change the portname according to your computer
                port = new SerialPort("COM6", 9600);
                port.Open();
                timer1.Enabled = true;
            }
        }
        void Form1_FormClosed(object sender, FormClosedEventArgs e)
        {
            if (port != null && port.IsOpen)
            {
                port.Close();
                timer1.Enabled = false;
            }
        }

        private void ManualMovementForward(object sender, EventArgs e)
        {
            port.Write("w");
        }
        private void ManualTurnRight(object sender, EventArgs e)
        {
            port.Write("d");
        }

        private void ManualTurnLeft(object sender, EventArgs e)
        {
            port.Write("a");
        }

        private void ManualMovementBackwards(object sender, EventArgs e)
        {
            port.Write("s");
        }

        private void Form1_Load(object sender, EventArgs e)
        {

        }
        
        private void MovementStopped(object sender, EventArgs e)
        {
            port.Write("e");
        }

        private void TextTimer(object sender, EventArgs e)
        {
            string incomingText = port.ReadExisting();
            if (incomingText != "" && incomingText != null)
            {

                    textData.AppendText(incomingText);
            }
        }

        private void AutoSearchRoomLeft(object sender, EventArgs e)
        {
            port.Write("z");
        }

        private void AutoSearchRoomRight(object sender, EventArgs e)
        {
            port.Write("x");
        }

        private void AutoTurnAroundTJunction(object sender, EventArgs e)
        {
            port.Write("b");
        }

        private void ReturnHome(object sender, EventArgs e)
        {
            port.Write("h");
        }
        private void ManualRotateRight(object sender, EventArgs e)
        {
            port.Write("r");
        }
        private void ManualRotatelLeft(object sender, EventArgs e)
        {
            port.Write("q");
        }

        private void ManualAutoMoveForward(object sender, EventArgs e)
        {
            port.Write("c");
        }

        private void AutomatedMovementForward(object sender, EventArgs e)
        {
            port.Write("i");
        }

        private void AutomatedRotateRight(object sender, EventArgs e)
        {
            port.Write("j");
        }

        private void AutomatedRotateLeft(object sender, EventArgs e)
        {
            port.Write("l");
        }

        private void AdjustForward(object sender, EventArgs e)
        {
            port.Write("t");
        }

        private void AdjustLeft(object sender, EventArgs e)
        {
            port.Write("f");
        }

        private void AdjustBackwards(object sender, EventArgs e)
        {
            port.Write("g");
        }

        private void AdjustRight(object sender, EventArgs e)
        {
            port.Write("y");
        }

        private void AutoStopButton(object sender, EventArgs e)
        {
            port.Write("e");
        }
    }
}