import unittest
import pexpect
import sys

class QemuTest(unittest.TestCase):
    def setUp(self):
        self.p = pexpect.spawn("make run platform=qemu")
        self.p.delaybeforesend = None
        #self.p.logfile = sys.stdout.buffer
        self.p.ignorecase = True

    def tearDown(self):
        self.p.terminate()


class BehaviorTest(QemuTest):
    def test_getppid(self):
        self.p.expect('-> / \$ ', timeout=3)
        self.p.sendline('getppid')
        self.p.expect(r'child, getppid: (\d+)')
        ppid = int(self.p.match.group(1))
        self.p.expect(r'parent, getpid: (\d+)')
        pid = int(self.p.match.group(1))
        self.assertEqual(pid, ppid)


class StringTest(unittest.TestCase):
    def test_upper(self):
        self.assertEqual("foo".upper(), "FOO")

    def test_lower(self):
        self.assertEqual("foo".upper(), "FOO")


if __name__ == "__main__":
    unittest.main()
