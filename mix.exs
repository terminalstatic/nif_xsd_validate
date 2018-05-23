defmodule NifXsdValidate.MixProject do
  use Mix.Project

  @version "0.0.9"

  def project do
    [
      app: :nif_xsd_validate,
      version: @version,
      elixir: "~> 1.6",
      package: [
        maintainers: [],
        licenses: ["terminalstatic"],
        links: %{"Github" => "https://github.com/terminalstatic/nif_xsd_validate"},
        files: [
          "mix.exs",
          "Makefile",
          "README.md",
          "config",
          "lib",
          "priv/.gitkeep",
          "c_source"
        ]
      ],
      compilers: [:nifXsdValidate] ++ Mix.compilers,
      start_permanent: Mix.env() == :prod,
      deps: deps(),
      #ExDoc
      name: "NifXsdValidate",
      source_url: "https://github.com/terminalstatic/nif_xsd_validate"
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
      {:ex_doc, "~> 0.18.3", only: :dev, runtime: false},
      {:dialyxir, "~> 0.5", only: [:dev], runtime: false}
      # {:dep_from_hexpm, "~> 0.3.0"},
      # {:dep_from_git, git: "https://github.com/elixir-lang/my_dep.git", tag: "0.1.0"},
    ]
  end
end
defmodule Mix.Tasks.Compile.NifXsdValidate do
  def run(_args) do
    if match? {:win32, _}, :os.type do
      IO.warn("Windows is not supported.")
      exit(1)
    else
      if File.exists?("./deps/nif_xsd_validate") do
         File.cd!("./deps/nif_xsd_validate", fn ->
            {_, errcode} = System.cmd("make", [], into: IO.stream(:stdio, :line))
            if errcode != 0 do
               Mix.raise("Mix task failed")
            end
         end)
      else
         {_, errcode} = System.cmd("make", [], into: IO.stream(:stdio, :line))
         if errcode != 0 do
            Mix.raise("Mix task failed")
         end
      end
    end
    :ok 
  end
end
