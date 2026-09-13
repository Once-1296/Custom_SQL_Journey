import subprocess

"""Test the command line interface."""
result = subprocess.run(['bsql', '--help'], capture_output=True, text=True)
assert result.returncode == 0
assert 'Usage' in result.stdout
print(result.stdout)