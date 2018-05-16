defmodule NifXsdValidate.MixProject do
  use Mix.Project

  @version "0.0.1"

  def project do
    [
      app: :nif_xsd_validate,
      version: @version,
      elixir: "~> 1.6",
      compilers: [:validateXsd] ++ Mix.compilers,
      start_permanent: Mix.env() == :prod,
      deps: deps()
    ]
  end

  # Run "mix help compile.app" to learn about applications.
  def application do
    [
      extra_applications: [:logger]
    ]
  end

  # Run "mix help deps" to learn about dependencies.
  defp deps do
    [
      # {:dep_from_hexpm, "~> 0.3.0"},
      # {:dep_from_git, git: "https://github.com/elixir-lang/my_dep.git", tag: "0.1.0"},
    ]
  end
end
defmodule Mix.Tasks.Compile.ValidateXsd do
  def run(_args) do
    {result, _errcode} = System.cmd("make", ["priv/validateXsd.so"], stderr_to_stdout: true)
    IO.binwrite(result)
  end
end
