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

        private void button1_Click(object sender, EventArgs e)
        {
            port.Write("w");
        }
        private void button2_Click(object sender, EventArgs e)
        {
            port.Write("d");
        }

        private void button3_Click(object sender, EventArgs e)
        {
            port.Write("a");
        }

        private void button4_Click(object sender, EventArgs e)
        {
            port.Write("s");
        }

        private void Form1_Load(object sender, EventArgs e)
        {

        }
        
        private void button5_Click(object sender, EventArgs e)
        {
            port.Write("e");
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            string incomingText = port.ReadExisting();
            if (incomingText != "" && incomingText != null)
            {
                textData.Text += "\n";
                textData.Text += incomingText + "\n";
            }
        }

        private void button6_Click(object sender, EventArgs e)
        {
            port.Write("z");
        }

        private void button7_Click(object sender, EventArgs e)
        {
            port.Write("x");
        }
    }
}